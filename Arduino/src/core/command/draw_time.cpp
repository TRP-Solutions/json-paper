#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"
#include "../../font/fonts.h"

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

        log(WARNING, "Missing required args for 'draw_time'");

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

        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_time'"
        );

        return;
        }
    Paint_DrawTime(x+1, y+1, &t, font, fg_c, bg_c);
}