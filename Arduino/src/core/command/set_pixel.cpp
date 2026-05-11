#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"

void SetPixel(const PaperCommand& command) {
    std::string xs, ys, cs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "color", cs)) {
        log(WARNING, "Missing required args for 'set_pixel'");

        return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t color;
    if (!ParseColor(cs, color)) return;


    if (x < 0 || y < 0 ||
    x >= Paint.Width ||
    y >= Paint.Height) {
        log(WARNING,"Input exceeds the normal display range for 'set_pixel'");

        return;
    }


    Paint_SetPixel(x+1, y+1, color);
}