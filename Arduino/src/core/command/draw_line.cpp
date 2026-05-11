#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"

void DrawLine(const PaperCommand& command) {
    std::string xs, ys, xe, ye, cs, ws, ss;

    if (!GetRequiredArg(command, "x_start", xs) ||
        !GetRequiredArg(command, "y_start", ys) ||
        !GetRequiredArg(command, "x_end", xe) ||
        !GetRequiredArg(command, "y_end", ye) ||
        !GetRequiredArg(command, "color", cs) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "style", ss)) {
        log(WARNING, "Missing required args for 'draw_line'");

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
        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_line'"
        );

        return;
    }

    Paint_DrawLine(x1+1, y1+1, x2+1, y2+1, color, width, style);
}