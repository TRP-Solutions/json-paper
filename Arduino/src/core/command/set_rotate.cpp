#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"

void SetRotate(const PaperCommand& command) {
    std::string val;
    if (!GetRequiredArg(command, "rotate", val))  {
        log(WARNING, "Missing required args for 'set_rotate'");

        return;
    }

    int rotate;
    if (!ParseInt(val, rotate)) return;

    Paint_SetRotate((UWORD)rotate);
}