#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"

void Clear(const PaperCommand& command) {
    std::string cs;
    if (!GetRequiredArg(command, "color", cs))  {
        log(WARNING, "Missing required args for 'clear'");

        return;
    }

    uint8_t color;
    if (!ParseColor(cs, color)) return;

    Paint_Clear(color);
}