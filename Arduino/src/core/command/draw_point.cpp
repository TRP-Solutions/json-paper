#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"

void DrawPoint(const PaperCommand& command) {
    std::string xs, ys, cs, ws, ss;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) {
        log(WARNING, "Missing required args for 'draw_point'");

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
        x >= Paint.Width || y >= Paint.Height) {
        log(WARNING, "Input exceeds the normal display range for 'draw_point'");

        return;
        }
    Paint_DrawPoint(x+1, y+1, color, width, style);
}