#define STB_TRUETYPE_IMPLEMENTATION
#include "../../font/stb_truetype.h"
#include "../../font/generated_font_registry.h"
#include "../core.h"
#include "../../e-paper/paper_command.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {
struct Glyph {
    int codepoint;
    size_t span;
    int advance;
    int kern;
};

struct Line {
    std::vector<Glyph> glyphs;
    int width = 0;
    int ascent = 0;
    int descent = 0;
};

struct LoadedFace {
    stbtt_fontinfo info{};
    bool initialized = false;
    bool valid = false;
};

const JsonPaperFontAsset* FindAsset(const std::string& family,
                                    const std::string& weight) {
    for (size_t index = 0; index < JSON_PAPER_FONT_COUNT; ++index) {
        const auto& asset = JSON_PAPER_FONTS[index];
        if (family == asset.family && weight == asset.weight) return &asset;
    }
    // A family with no bold file remains usable; bold is synthesized later.
    if (weight == "bold") return FindAsset(family, "regular");
    return nullptr;
}

stbtt_fontinfo* Face(const std::string& family, const std::string& weight) {
    const JsonPaperFontAsset* asset = FindAsset(family, weight);
    if (!asset) return nullptr;
    static std::vector<LoadedFace> faces(JSON_PAPER_FONT_COUNT);
    size_t index = static_cast<size_t>(asset - JSON_PAPER_FONTS);
    auto& loaded = faces[index];
    if (!loaded.initialized) {
        int offset = stbtt_GetFontOffsetForIndex(asset->data, 0);
        loaded.valid = offset >= 0 && stbtt_InitFont(&loaded.info, asset->data, offset);
        loaded.initialized = true;
    }
    return loaded.valid ? &loaded.info : nullptr;
}

int NextCodepoint(const std::string& value, size_t& offset) {
    const auto* bytes = reinterpret_cast<const unsigned char*>(value.data());
    unsigned char first = bytes[offset++];
    if (first < 0x80) return first;
    int count = 0;
    int cp = 0;
    if ((first & 0xe0) == 0xc0) { count = 1; cp = first & 0x1f; }
    else if ((first & 0xf0) == 0xe0) { count = 2; cp = first & 0x0f; }
    else if ((first & 0xf8) == 0xf0) { count = 3; cp = first & 0x07; }
    else return 0xfffd;
    if (offset + count > value.size()) { offset = value.size(); return 0xfffd; }
    for (int i = 0; i < count; ++i) {
        unsigned char next = bytes[offset];
        if ((next & 0xc0) != 0x80) return 0xfffd;
        ++offset;
        cp = (cp << 6) | (next & 0x3f);
    }
    if ((count == 1 && cp < 0x80) || (count == 2 && cp < 0x800) ||
        (count == 3 && cp < 0x10000) || cp > 0x10ffff ||
        (cp >= 0xd800 && cp <= 0xdfff)) return 0xfffd;
    return cp;
}

uint8_t SpanColor(const std::string& value) {
    if (value == "red") return EPD_5in79G_RED;
    if (value == "yellow") return EPD_5in79G_YELLOW;
    if (value == "white") return EPD_5in79G_WHITE;
    return EPD_5in79G_BLACK;
}

void AddLine(std::vector<Line>& lines) {
    lines.emplace_back();
}

std::vector<Line> Layout(const PaperTextCommand& text) {
    std::vector<Line> lines(1);
    int previous = 0;
    stbtt_fontinfo* previousFace = nullptr;
    for (size_t spanIndex = 0; spanIndex < text.spans.size(); ++spanIndex) {
        const auto& span = text.spans[spanIndex];
        stbtt_fontinfo* face = Face(span.family, span.weight);
        if (!face) continue;
        if (face != previousFace) previous = 0;
        float scale = stbtt_ScaleForPixelHeight(face, static_cast<float>(span.size));
        int rawAscent, rawDescent, gap;
        stbtt_GetFontVMetrics(face, &rawAscent, &rawDescent, &gap);
        int ascent = static_cast<int>(std::ceil(rawAscent * scale));
        int descent = static_cast<int>(std::ceil(-rawDescent * scale));
        for (size_t offset = 0; offset < span.text.size();) {
            int cp = NextCodepoint(span.text, offset);
            if (cp == '\r') continue;
            if (cp == '\t') {
                // The wire contract defines a tab as four ordinary spaces.
                int rawAdvance, bearing;
                stbtt_GetCodepointHMetrics(face, ' ', &rawAdvance, &bearing);
                int advance = std::max(0, static_cast<int>(std::round(rawAdvance * scale))
                    + span.letterSpacing);
                for (int tabSpace = 0; tabSpace < 4; ++tabSpace) {
                    Line* line = &lines.back();
                    if (!line->glyphs.empty() && line->width + advance > text.width) {
                        AddLine(lines); line = &lines.back();
                    }
                    line->glyphs.push_back({' ', spanIndex, advance, 0});
                    line->width += advance;
                    line->ascent = std::max(line->ascent, ascent);
                    line->descent = std::max(line->descent, descent);
                }
                previous = ' ';
                previousFace = face;
                continue;
            }
            if (cp == '\n') {
                AddLine(lines); previous = 0; continue;
            }
            if (stbtt_FindGlyphIndex(face, cp) == 0) cp = 0xfffd;
            int rawAdvance, bearing;
            stbtt_GetCodepointHMetrics(face, cp, &rawAdvance, &bearing);
            int kern = previous
                ? static_cast<int>(std::round(stbtt_GetCodepointKernAdvance(face, previous, cp) * scale))
                : 0;
            int advance = std::max(0, static_cast<int>(std::round(rawAdvance * scale))
                + span.letterSpacing);
            Line* line = &lines.back();
            if (!line->glyphs.empty() && line->width + kern + advance > text.width) {
                AddLine(lines); line = &lines.back(); kern = 0; previous = 0;
            }
            line->glyphs.push_back({cp, spanIndex, advance, kern});
            line->width += kern + advance;
            line->ascent = std::max(line->ascent, ascent);
            line->descent = std::max(line->descent, descent);
            previous = cp;
            previousFace = face;
        }
    }
    return lines;
}

void PaintGlyph(const PaperTextSpan& span, int cp, int penX, int baseline,
                const PaperTextCommand& box) {
    stbtt_fontinfo* face = Face(span.family, span.weight);
    if (!face) return;
    float scale = stbtt_ScaleForPixelHeight(face, static_cast<float>(span.size));
    int width, height, xOffset, yOffset;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        face, 0, scale, cp, &width, &height, &xOffset, &yOffset);
    if (!bitmap) return;
    uint8_t color = SpanColor(span.color);
    const JsonPaperFontAsset* selected = FindAsset(span.family, span.weight);
    bool syntheticBold = span.weight == "bold" && selected &&
        std::string(selected->weight) != "bold";
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            if (bitmap[py * width + px] < 96) continue;
            int x = penX + xOffset + px;
            int y = baseline + yOffset + py;
            if (x < 0 || y < 0 || x >= Paint.Width || y >= Paint.Height ||
                x < box.x || y < box.y || x >= box.x + box.width ||
                y >= box.y + box.height) continue;
            Paint_DrawPoint(x, y, color, DOT_PIXEL_1X1, DOT_STYLE_DFT);
            // The bundled face has one weight. Bold is synthesized identically
            // on both targets by adding one coverage pixel horizontally.
            if (syntheticBold && x + 1 < box.x + box.width)
                Paint_DrawPoint(x + 1, y, color, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        }
    }
    stbtt_FreeBitmap(bitmap, nullptr);
}
} // namespace

void DrawText(const PaperCommand& command) {
    const auto& text = command.text;
    if (text.width <= 0 || text.height <= 0 || text.spans.empty()) {
        log(WARNING, "Malformed 'draw_text': positive bounds and spans are required");
        return;
    }
    for (const auto& span : text.spans) {
        if (span.size < 6 || span.size > 96 || !FindAsset(span.family, span.weight) ||
            (span.weight != "regular" && span.weight != "bold")) {
            log(WARNING, "Malformed 'draw_text': unsupported font style or size");
            return;
        }
    }
    if (text.background != "transparent") {
        uint8_t color;
        if (!ParseColor(text.background, color)) return;
        Paint_ClearWindows(text.x, text.y, text.x + text.width,
                           text.y + text.height, color);
    }

    auto lines = Layout(text);
    int totalHeight = 0;
    size_t visible = 0;
    for (const auto& line : lines) {
        int lineHeight = std::max(1, line.ascent + line.descent);
        if (visible && totalHeight + lineHeight > text.height) break;
        totalHeight += lineHeight + text.lineSpacing;
        ++visible;
    }
    if (visible) totalHeight -= text.lineSpacing;
    int y = text.y;
    if (text.verticalAlign == "middle") y += (text.height - totalHeight) / 2;
    else if (text.verticalAlign == "bottom") y += text.height - totalHeight;

    for (size_t lineIndex = 0; lineIndex < visible; ++lineIndex) {
        const auto& line = lines[lineIndex];
        int x = text.x;
        if (text.horizontalAlign == "center") x += (text.width - line.width) / 2;
        else if (text.horizontalAlign == "right") x += text.width - line.width;
        int baseline = y + line.ascent;
        for (const auto& glyph : line.glyphs) {
            x += glyph.kern;
            const auto& span = text.spans[glyph.span];
            PaintGlyph(span, glyph.codepoint, x, baseline, text);
            uint8_t color = SpanColor(span.color);
            if (span.underline)
                Paint_DrawLine(x, baseline + 1, x + glyph.advance - 1, baseline + 1,
                               color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            if (span.strikeout)
                Paint_DrawLine(x, baseline - span.size / 3,
                               x + glyph.advance - 1, baseline - span.size / 3,
                               color, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
            x += glyph.advance;
        }
        y += std::max(1, line.ascent + line.descent) + text.lineSpacing;
    }
}
