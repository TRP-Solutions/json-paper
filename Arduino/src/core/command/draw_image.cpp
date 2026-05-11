#include <vector>

#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"

std::vector<uint8_t> Base64Decode(const std::string & string);

void DrawImage(const PaperCommand& command) {
    std::string xs, ys, ws, hs, dataStr, transparentStr;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "width", ws) ||
        !GetRequiredArg(command, "height", hs) ||
        !GetRequiredArg(command, "data", dataStr)) {
            log(WARNING, "Missing required args for 'draw_image'");

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
        log(WARNING, "Invalid image data size");

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
                log(
                    WARNING,
                    "Input exceeds the normal display range for 'draw_image'"
                );

                return;
            }

            Paint_DrawPoint(
                x + sx+1,
                y + sy+1,
                pixel,
                DOT_PIXEL_1X1,
                DOT_STYLE_DFT
            );
        }
    }
}