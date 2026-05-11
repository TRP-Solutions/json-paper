#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"

void DrawCircle(const PaperCommand& command) {
    std::string xs, ys, rs, cs, ws, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "radius", rs) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "fill", fs)) {
        log(WARNING, "Missing required args for 'draw_circle'");

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
        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_circle'"
        );

        return;
    }

    Paint_DrawCircle(x+1, y+1, r, color, width, fill);
}