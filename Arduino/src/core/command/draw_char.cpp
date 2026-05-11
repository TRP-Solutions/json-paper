#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"
#include "../../font/fonts.h"

void DrawChar(const PaperCommand& command) {
    std::string xs, ys, chs, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "char", chs) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs)) {
        log(WARNING, "Missing required args for 'draw_char'");

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

        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_char'"
        );

        return;
        }

    Paint_DrawChar(x+1, y+1, chs[0], font, fg_c, bg_c);
}