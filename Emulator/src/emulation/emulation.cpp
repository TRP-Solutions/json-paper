/*****************************************************************************
* | File      	:   EPD_5in79g_test.c
* | Author      :   Waveshare team
* | Function    :   5.79inch e-paper (G) test demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2024-03-19
* | Info        :
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "manager/console_manager.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define NOGDI
#define NOUSER

#include <httplib.h>
#include <vector>

#include "epd_test.h"

#include "../network/request.h"

#include "GUI/gui_raylib.h"
#include "e-Paper/EPD_5in79g.h"
#include "e-Paper/paper_command.h"

int simulate_epd_5in79g_test(std::string link) {

    //Create a new image cache named IMAGE_BW and fill it with white
    uint8_t *BlackImage;
    UWORD Imagesize = ((EPD_5in79G_WIDTH + 3) / 4) * EPD_5in79G_HEIGHT;

    if((BlackImage = (uint8_t *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }

    Debug("NewImage:BlackImage and RYImage\r\n");
    Paint_NewImage(BlackImage, EPD_5in79G_WIDTH/2, EPD_5in79G_HEIGHT/2, 0, WHITE);
    Paint_SetScale(4);

    //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

#if 1   // Drawing on the image
    //1.Select Image
    Debug("SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_5in79G_WHITE);

    // 2.Drawing on the image
    Debug("Drawing:BlackImage\r\n");

    draw_epd_5in79g_remote(link);

    Debug("EPD_Display\r\n");
    GUI_Raylib::CopyToDisplayBuffer(BlackImage, EPD_5in79G_WIDTH/2, EPD_5in79G_HEIGHT/2,
                             (EPD_5in79G_WIDTH/2 + 3) / 4);
#endif
    free(BlackImage);
    BlackImage = NULL;

    return 0;
}

void draw_epd_5in79g_remote(std::string link) {
    std::vector<PaperCommand> commands = Request::RequestConfig(link);

    for (const auto& command : commands) {
        execute_command(command);
    }
}

// -------------------- HELPERS --------------------

bool ParseColor(const std::string& color, uint8_t& out) {
    if (colorMap.count(color)) {
        out = colorMap[color];
        return true;
    }
    ConsoleManager::get().log(WARNING, ("invalid color: " + color).c_str());
    return false;
}

bool GetRequiredArg(const PaperCommand& cmd, const std::string& key, std::string& out) {
    auto it = cmd.args.find(key);
    if (it == cmd.args.end()) {
        ConsoleManager::get().log(WARNING, ("missing '" + key + "'").c_str());
        return false;
    }
    out = it->second;
    return true;
}

bool GetOptionalArg(const PaperCommand& cmd, const std::string& key, std::string& out) {
    auto it = cmd.args.find(key);
    if (it == cmd.args.end()) {
        return false;
    }
    out = it->second;
    return true;
}

bool ParseInt(const std::string& s, int& out) {
    try {
        out = std::stoi(s);
        return true;
    } catch (...) {
        ConsoleManager::get().log(WARNING, ("invalid int: " + s).c_str());
        return false;
    }
}

bool ParseDotPixel(const std::string& s, DOT_PIXEL& out) {
    if (s == "1x1") out = DOT_PIXEL_1X1;
    else if (s == "2x2") out = DOT_PIXEL_2X2;
    else if (s == "3x3") out = DOT_PIXEL_3X3;
    else if (s == "4x4") out = DOT_PIXEL_4X4;
    else {
        ConsoleManager::get().log(WARNING, ("invalid width: " + s).c_str());
        return false;
    }
    return true;
}

bool ParseLineStyle(const std::string& s, LINE_STYLE& out) {
    if (lineStyleMap.count(s)) {
        out = lineStyleMap[s];
        return true;
    }
    ConsoleManager::get().log(WARNING, ("invalid line style: " + s).c_str());
    return false;
}
bool ParseDrawFill(const std::string& s, DRAW_FILL& out) {
    if (drawFillMap.count(s)) {
        out = drawFillMap[s];
        return true;
    }
    ConsoleManager::get().log(WARNING, ("invalid fill: " + s).c_str());
    return false;
}
bool ParseDotStyle(const std::string& s, DOT_STYLE& out) {
    if (dotStyleMap.count(s)) {
        out = dotStyleMap[s];
        return true;
    }
    ConsoleManager::get().log(WARNING, ("invalid dot style: " + s).c_str());
    return false;
}
bool ParseFont(const std::string& s, sFONT*& out) {
    if (fontMap.count(s)) {
        out = fontMap[s];
        return true;
    }
    ConsoleManager::get().log(WARNING, ("invalid font: " + s).c_str());

    return false;
}

// -------------------- COMMANDS --------------------

void Clear(const PaperCommand& command) {
    std::string cs;
    if (!GetRequiredArg(command, "color", cs))  {
        ConsoleManager::get().log(WARNING, "Missing required args for 'clear'");
        return;
    }

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    Paint_Clear(color);
}

void SetRotate(const PaperCommand& command) {
    std::string val;
    if (!GetRequiredArg(command, "rotate", val))  {
        ConsoleManager::get().log(WARNING, "Missing required args for 'set_rotate'");
        return;
    }

    int rotate;
    if (!ParseInt(val, rotate)) return;

    Paint_SetRotate((UWORD)rotate);
}

void SetMirroring(const PaperCommand& command) {
    std::string val;
    if (!GetRequiredArg(command, "mirror", val))  {
        ConsoleManager::get().log(WARNING, "Missing required args for 'set_mirroring'");
        return;
    }

    int mirror;
    if (!ParseInt(val, mirror)) return;

    Paint_SetMirroring((UBYTE)mirror);
}

void SetPixel(const PaperCommand& command) {
    std::string xs, ys, cs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'set_pixel'");
            return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    ConsoleManager::get().log(WARNING, cs.c_str());

    Paint_SetPixel(x, y, color);
}

void ClearWindow(const PaperCommand& command) {
    std::string xs, ys, xe, ye, cs;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'clear_window'");
            return;
        }

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    if (x1 < 0 || y1 < 0 ||
        x2 < 0 || y2 < 0 ||
        x1 >= Paint.Width || y1 >= Paint.Height ||
        x2 >= Paint.Width || y2 >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'clear_window'"
        );
        return;
    }

    Paint_ClearWindows(x1, y1, x2, y2, color);
}

// -------------------- DRAW --------------------

void DrawPoint(const PaperCommand& command) {
    std::string xs, ys, cs, ws, ss;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_point'");
            return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t color;
    DOT_PIXEL width;
    DOT_STYLE style;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDotStyle(ss, style)) return;

    if (x < 0 || y < 0 ||
        x > Paint.Width || y > Paint.Height) {
        ConsoleManager::get().log(WARNING, "Input exceeds the normal display range for 'draw_point'");
        return;
    }
    Paint_DrawPoint(x, y, color, width, style);
}

void DrawLine(const PaperCommand& command) {
    std::string xs, ys, xe, ye, cs, ws, ss;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_line'");
            return;
        }

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    DOT_PIXEL width;
    LINE_STYLE style;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseLineStyle(ss, style)) return;

    if (x1 < 0 || y1 < 0 ||
    x2 < 0 || y2 < 0 ||
    x1 >= Paint.Width || y1 >= Paint.Height ||
    x2 >= Paint.Width || y2 >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_line'"
        );
        return;
    }

    Paint_DrawLine(x1, y1, x2, y2, color, width, style);
}

void DrawRectangle(const PaperCommand& command) {
    std::string xs, ys, xe, ye, cs, ws, fs;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "fill", fs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_rectangle'");
            return;
        }

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    DOT_PIXEL width;
    DRAW_FILL fill;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDrawFill(fs, fill)) return;

    if (x1 < 0 || y1 < 0 ||
        x2 < 0 || y2 < 0 ||
        x1 >= Paint.Width || y1 >= Paint.Height ||
        x2 >= Paint.Width || y2 >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_rectangle'"
        );
        return;
    }
    Paint_DrawRectangle(x1, y1, x2, y2, color, width, fill);
}

void DrawCircle(const PaperCommand& command) {
    std::string xs, ys, rs, cs, ws, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "radius", rs) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "fill", fs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_circle'");
            return;
        }

    int x, y, r;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) || !ParseInt(rs, r)) return;

    uint8_t color;
    DOT_PIXEL width;
    DRAW_FILL fill;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDrawFill(fs, fill)) return;

    if (r < 0 ||
    x - r < 0 ||
    y - r < 0 ||
    x + r >= Paint.Width ||
    y + r >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_circle'"
        );
        return;
    }

    Paint_DrawCircle(x, y, r, color, width, fill);
}

std::vector<uint8_t> Base64Decode(const std::string & string);

void DrawImage(const PaperCommand& command) {
    std::string xs, ys, ws, hs, dataStr, transparentStr;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "height", hs) ||
        !GetRequiredArg(command, "data", dataStr)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_image'");
            return;
    }

    int x, y, width, height;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) ||
        !ParseInt(ws, width) || !ParseInt(hs, height)) return;

    int transparent = 4;
    if (GetOptionalArg(command, "transparent", transparentStr)) {
        ParseInt(transparentStr, transparent);
    }

    std::vector<uint8_t> buffer = Base64Decode(dataStr);

    int expected = (width * height + 1) / 2;
    if ((int)buffer.size() != expected) {
        ConsoleManager::get().log(WARNING, "Invalid image data size");
        return;
    }

    for (int sy = 0; sy < height; sy++) {
        for (int sx = 0; sx < width; sx++) {

            int index = sy * width + sx;
            int byteIndex = index / 2;

            uint8_t byte = buffer[byteIndex];
            uint8_t pixel;

            if ((index % 2) == 0) {
                // high nibble
                pixel = (byte >> 4) & 0x0F;
            } else {
                // low nibble
                pixel = byte & 0x0F;
            }

            if (transparent != -1 && pixel == transparent)
                continue;

            if (x < 0 || y < 0 ||
            width <= 0 || height <= 0) {
                ConsoleManager::get().log(
                    WARNING,
                    "Input exceeds the normal display range for 'draw_image'"
                );
                return;
            }

            Paint_DrawPoint(
                x + sx,
                y + sy,
                pixel,
                DOT_PIXEL_1X1,
                DOT_STYLE_DFT
            );
        }
    }
}

// -------------------- TEXT --------------------

void DrawChar(const PaperCommand& command) {
    std::string xs, ys, chs, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "char", chs) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_char'");
            return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    if (x < 0 || y < 0 ||
        x >= Paint.Width ||
        y >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_char'"
        );
        return;
    }

    Paint_DrawChar(x, y, chs[0], font, fg_c, bg_c);
}

void DrawNum(const PaperCommand& command) {
    std::string xs, ys, ns, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "num", ns) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs))  {
        ConsoleManager::get().log(WARNING, "Missing required args for 'draw_num'");
        return;
        }

    int x, y, num;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) || !ParseInt(ns, num)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    if (x < 0 || y < 0 ||
        x >= Paint.Width ||
        y >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_num'"
        );
        return;
    }
    Paint_DrawNum(x, y, num, font, fg_c, bg_c);
}

void DrawTime(const PaperCommand& command) {
    std::string xs, ys, hs, ms, ss, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "hour", hs) ||
        !GetRequiredArg(command, "min", ms) ||
        !GetRequiredArg(command, "sec", ss) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_time'");
            return;
        }

    int x, y, h, m, s;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) ||
        !ParseInt(hs, h) || !ParseInt(ms, m) || !ParseInt(ss, s)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    PAINT_TIME t{(UBYTE)h, (UBYTE)m, (UBYTE)s};

    if (x < 0 || y < 0 ||
        x >= Paint.Width ||
        y >= Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_time'"
        );
        return;
    }
    Paint_DrawTime(x, y, &t, font, fg_c, bg_c);
}


void Paint_DrawChar_Transparent(UWORD Xstart, UWORD Ystart, const char Acsii_Char,
                               sFONT* Font, UWORD Color_Foreground)
{
    UWORD Page, Column;

    const uint8_t *ptr = &Font->table[(Acsii_Char - ' ') * Font->Height * ((Font->Width + 7) / 8)];

    for (Page = 0; Page < Font->Height; Page++) {
        for (Column = 0; Column < Font->Width; Column++) {

            if (ptr[Column / 8] & (0x80 >> (Column % 8))) {
                Paint_SetPixel(Xstart + Column, Ystart + Page, Color_Foreground);
            }

        }
        ptr += (Font->Width + 7) / 8;
    }
}

void Paint_DrawString_EN_Transparent(UWORD Xstart, UWORD Ystart, const char *pString,
                                     sFONT* Font, UWORD Color_Foreground)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height) {
        Debug("Paint_DrawString_EN_Transparent out of range\r\n");
        return;
    }

    while (*pString != '\0') {

        if ((Xpoint + Font->Width) > Paint.Width) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        if ((Ypoint + Font->Height) > Paint.Height) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }

        // Draw character WITHOUT background
        Paint_DrawChar_Transparent(Xpoint, Ypoint, *pString, Font, Color_Foreground);

        pString++;
        Xpoint += Font->Width;
    }
}

void DrawString(const PaperCommand& command) {
    std::string xs, ys, text, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "text", text) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs))  {
            ConsoleManager::get().log(WARNING, "Missing required args for 'draw_string'");
            return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t fg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseFont(fs, font)) return;

    if (x < 0 || y < 0 ||
    x + font->Width > Paint.Width ||
    y + font->Height > Paint.Height) {

        ConsoleManager::get().log(
            WARNING,
            "Input exceeds the normal display range for 'draw_string'"
        );
        return;
    }

    if (bg == "transparent") {
        Paint_DrawString_EN_Transparent(x, y, text.c_str(), font, fg_c);
        return;
    }

    uint8_t bg_c;
    if (!ParseColor(bg, bg_c)) return;

    Paint_DrawString_EN(x, y, text.c_str(), font, fg_c, bg_c);
}

