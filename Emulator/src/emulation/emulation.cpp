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
#include <httplib.h>
#include <vector>

#include "epd_test.h"

#include "../network/request.h"

#include "GUI/gui_raylib.h"
#include "e-Paper/EPD_5in79g.h"
#include "e-Paper/command.h"

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
    std::vector<command> commands = Request::RequestConfig(link);

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
    std::cout << "invalid color: " << color << "\n";
    return false;
}

bool GetRequiredArg(const command& cmd, const std::string& key, std::string& out) {
    auto it = cmd.args.find(key);
    if (it == cmd.args.end()) {
        std::cout << "missing '" << key << "'\n";
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
        std::cout << "invalid int: " << s << "\n";
        return false;
    }
}

bool ParseDotPixel(const std::string& s, DOT_PIXEL& out) {
    if (s == "1x1") out = DOT_PIXEL_1X1;
    else if (s == "2x2") out = DOT_PIXEL_2X2;
    else if (s == "3x3") out = DOT_PIXEL_3X3;
    else if (s == "4x4") out = DOT_PIXEL_4X4;
    else {
        std::cout << "invalid width: " << s << "\n";
        return false;
    }
    return true;
}

bool ParseLineStyle(const std::string& s, LINE_STYLE& out) {
    if (lineStyleMap.count(s)) {
        out = lineStyleMap[s];
        return true;
    }
    std::cout << "invalid line style: " << s << "\n";
    return false;
}
bool ParseDrawFill(const std::string& s, DRAW_FILL& out) {
    if (drawFillMap.count(s)) {
        out = drawFillMap[s];
        return true;
    }
    std::cout << "invalid fill: " << s << "\n";
    return false;
}
bool ParseDotStyle(const std::string& s, DOT_STYLE& out) {
    if (dotStyleMap.count(s)) {
        out = dotStyleMap[s];
        return true;
    }
    std::cout << "invalid dot style: " << s << "\n";
    return false;
}
bool ParseFont(const std::string& s, sFONT*& out) {
    if (fontMap.count(s)) {
        out = fontMap[s];
        return true;
    }
    std::cout << "invalid font: " << s << "\n";
    return false;
}

// -------------------- COMMANDS --------------------

void Clear(const command& command) {
    std::string cs;
    if (!GetRequiredArg(command, "color", cs)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    Paint_Clear(color);
}

void SetRotate(const command& command) {
    std::string val;
    if (!GetRequiredArg(command, "rotate", val)) return;

    int rotate;
    if (!ParseInt(val, rotate)) return;

    Paint_SetRotate((UWORD)rotate);
}

void SetMirroring(const command& command) {
    std::string val;
    if (!GetRequiredArg(command, "mirror", val)) return;

    int mirror;
    if (!ParseInt(val, mirror)) return;

    Paint_SetMirroring((uint8_t)mirror);
}

void SetPixel(const command& command) {
    std::string xs, ys, cs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs)) return;

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    Paint_SetPixel(x, y, color);
}

void ClearWindow(const command& command) {
    std::string xs, ys, xe, ye, cs;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs)) return;

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    Paint_ClearWindows(x1, y1, x2, y2, color);
}

// -------------------- DRAW --------------------

void DrawPoint(const command& command) {
    std::string xs, ys, cs, ws, ss;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) return;

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t color;
    DOT_PIXEL width;
    DOT_STYLE style;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDotStyle(ss, style)) return;

    Paint_DrawPoint(x, y, color, width, style);
}

void DrawLine(const command& command) {
    std::string xs, ys, xe, ye, cs, ws, ss;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) return;

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    DOT_PIXEL width;
    LINE_STYLE style;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseLineStyle(ss, style)) return;

    Paint_DrawLine(x1, y1, x2, y2, color, width, style);
}

void DrawRectangle(const command& command) {
    std::string xs, ys, xe, ye, cs, ws, fs;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "fill", fs)) return;

    int x1, y1, x2, y2;
    if (!ParseInt(xs, x1) || !ParseInt(ys, y1) ||
        !ParseInt(xe, x2) || !ParseInt(ye, y2)) return;

    uint8_t color;
    DOT_PIXEL width;
    DRAW_FILL fill;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDrawFill(fs, fill)) return;

    Paint_DrawRectangle(x1, y1, x2, y2, color, width, fill);
}

void DrawCircle(const command& command) {
    std::string xs, ys, rs, cs, ws, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "radius", rs) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "fill", fs)) return;

    int x, y, r;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) || !ParseInt(rs, r)) return;

    uint8_t color;
    DOT_PIXEL width;
    DRAW_FILL fill;

    if (!ParseColor(cs, color) ||
        !ParseDotPixel(ws, width) ||
        !ParseDrawFill(fs, fill)) return;

    Paint_DrawCircle(x, y, r, color, width, fill);
}

// -------------------- TEXT --------------------

void DrawChar(const command& command) {
    std::string xs, ys, chs, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "char", chs) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) return;

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    Paint_DrawChar(x, y, chs[0], font, fg_c, bg_c);
}

void DrawNum(const command& command) {
    std::string xs, ys, ns, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "num", ns) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) return;

    int x, y, num;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) || !ParseInt(ns, num)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    Paint_DrawNum(x, y, num, font, fg_c, bg_c);
}

void DrawTime(const command& command) {
    std::string xs, ys, hs, ms, ss, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "hour", hs) ||
        !GetRequiredArg(command, "min", ms) ||
        !GetRequiredArg(command, "sec", ss) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) return;

    int x, y, h, m, s;
    if (!ParseInt(xs, x) || !ParseInt(ys, y) ||
        !ParseInt(hs, h) || !ParseInt(ms, m) || !ParseInt(ss, s)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    PAINT_TIME t{(UBYTE)h, (UBYTE)m, (UBYTE)s};

    Paint_DrawTime(x, y, &t, font, fg_c, bg_c);
}

void DrawString(const command& command) {
    std::string xs, ys, text, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "text", text) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) return;

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t fg_c, bg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseColor(bg, bg_c) ||
        !ParseFont(fs, font)) return;

    Paint_DrawString_EN(x, y, text.c_str(), font, fg_c, bg_c);
}