#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"
#include "font/fonts.h"

void DrawNum(const PaperCommand& command) {
    std::string xs, ys, ns, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "num", ns) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs))  {

        log(WARNING, "Missing required args for 'draw_num'");

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

        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_num'"
        );

        return;
        }
    Paint_DrawNum(x, y, num, font, fg_c, bg_c);
}