package solutions.trp.jsonpaper;

import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

/** Host-side fallback renderer. Its output still passes through PaperImage's four-colour quantizer. */
final class RasterTextRenderer {
    private record Glyph(int codePoint, TextSpan span, Font font, int advance) {}
    private record Line(List<Glyph> glyphs, int width, int ascent, int descent) {}

    private RasterTextRenderer() {}

    static BufferedImage render(TextBox box, RasterFontFamily family) {
        if (family == null) throw new NullPointerException("fonts");
        try {
            Font regular = load(family.regular());
            Font bold = load(family.bold());
            BufferedImage image = new BufferedImage(box.width(), box.height(), BufferedImage.TYPE_INT_ARGB);
            Graphics2D g = image.createGraphics();
            g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, RenderingHints.VALUE_TEXT_ANTIALIAS_ON);
            if (box.background() != Color.TRANSPARENT) {
                g.setColor(awt(box.background()));
                g.fillRect(0, 0, box.width(), box.height());
            }
            List<Line> lines = layout(box, g, regular, bold);
            int totalHeight = lines.stream().mapToInt(l -> l.ascent + l.descent).sum()
                + Math.max(0, lines.size() - 1) * box.lineSpacing();
            int y = switch (box.verticalAlign()) {
                case TOP -> 0;
                case MIDDLE -> (box.height() - totalHeight) / 2;
                case BOTTOM -> box.height() - totalHeight;
            };
            g.setClip(0, 0, box.width(), box.height());
            for (Line line : lines) {
                if (y + line.ascent + line.descent > box.height()) break;
                int x = switch (box.horizontalAlign()) {
                    case LEFT -> 0;
                    case CENTER -> (box.width() - line.width) / 2;
                    case RIGHT -> box.width() - line.width;
                };
                int baseline = y + line.ascent;
                for (Glyph glyph : line.glyphs) {
                    g.setFont(glyph.font);
                    g.setColor(awt(glyph.span.color()));
                    g.drawString(new String(Character.toChars(glyph.codePoint)), x, baseline);
                    FontMetrics metrics = g.getFontMetrics();
                    if (glyph.span.underline()) g.drawLine(x, baseline + 1, x + glyph.advance - 1, baseline + 1);
                    if (glyph.span.strikeout()) g.drawLine(x, baseline - metrics.getAscent() / 3, x + glyph.advance - 1, baseline - metrics.getAscent() / 3);
                    x += glyph.advance;
                }
                y += line.ascent + line.descent + box.lineSpacing();
            }
            g.dispose();
            return image;
        } catch (IOException | java.awt.FontFormatException exception) {
            throw new IllegalArgumentException("Could not load raster font family", exception);
        }
    }

    private static Font load(java.nio.file.Path path) throws IOException, java.awt.FontFormatException {
        try (InputStream input = java.nio.file.Files.newInputStream(path)) {
            return Font.createFont(Font.TRUETYPE_FONT, input);
        }
    }

    private static List<Line> layout(TextBox box, Graphics2D g, Font regular, Font bold) {
        List<Line> result = new ArrayList<>();
        List<Glyph> current = new ArrayList<>();
        int width = 0, ascent = 0, descent = 0;
        for (TextSpan span : box.spans()) {
            String normalized = span.text().replace("\r", "").replace("\t", "    ");
            Font font = (span.weight() == FontWeight.BOLD ? bold : regular).deriveFont((float) span.size());
            g.setFont(font);
            FontMetrics fm = g.getFontMetrics();
            for (int offset = 0; offset < normalized.length();) {
                int cp = normalized.codePointAt(offset);
                offset += Character.charCount(cp);
                if (cp == '\n') {
                    result.add(new Line(List.copyOf(current), width, Math.max(1, ascent), Math.max(0, descent)));
                    current.clear(); width = ascent = descent = 0; continue;
                }
                if (!font.canDisplay(cp)) cp = 0xfffd;
                int advance = fm.stringWidth(new String(Character.toChars(cp))) + span.letterSpacing();
                advance = Math.max(0, advance);
                if (!current.isEmpty() && width + advance > box.width()) {
                    result.add(new Line(List.copyOf(current), width, ascent, descent));
                    current.clear(); width = ascent = descent = 0;
                }
                current.add(new Glyph(cp, span, font, advance));
                width += advance; ascent = Math.max(ascent, fm.getAscent()); descent = Math.max(descent, fm.getDescent());
            }
        }
        if (!current.isEmpty() || result.isEmpty()) result.add(new Line(List.copyOf(current), width, Math.max(1, ascent), Math.max(0, descent)));
        return result;
    }

    private static java.awt.Color awt(Color color) {
        return switch (color) {
            case BLACK -> java.awt.Color.BLACK;
            case WHITE -> java.awt.Color.WHITE;
            case YELLOW -> java.awt.Color.YELLOW;
            case RED -> java.awt.Color.RED;
            case TRANSPARENT -> new java.awt.Color(0, 0, 0, 0);
        };
    }
}
