#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"

void SetMirroring(const PaperCommand& command) {
    std::string val;
    if (!GetRequiredArg(command, "mirror", val))  {
        log(WARNING, "Missing required args for 'set_mirroring'");

        return;
    }

    int mirror;
    if (!ParseInt(val, mirror)) return;

    Paint_SetMirroring((UBYTE)mirror);
}