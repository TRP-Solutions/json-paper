#include "paper_command.h"

#include "../core/core.h"

std::unordered_map<std::string, CmdHandler> cmdMap = {
    {"clear", Clear},
    {"clear_window", ClearWindow},

    {"set_rotate", SetRotate},
    {"set_mirroring", SetMirroring},
    {"set_pixel", SetPixel},

    {"draw_point", DrawPoint},
    {"draw_line", DrawLine},
    {"draw_rectangle", DrawRectangle},
    {"draw_circle", DrawCircle},
    {"draw_pie_slice", DrawPieSlice},
    {"draw_image", DrawImage},
    {"draw_text", DrawText}
};
std::unordered_map<std::string, uint8_t> colorMap = {
    {"white", EPD_5in79G_WHITE},
    {"black", EPD_5in79G_BLACK},
    {"red", EPD_5in79G_RED},
    {"yellow", EPD_5in79G_YELLOW}
};

std::unordered_map<std::string, LINE_STYLE> lineStyleMap = {
    {"solid", LINE_STYLE_SOLID},
    {"dotted", LINE_STYLE_DOTTED}
};

std::unordered_map<std::string, DRAW_FILL> drawFillMap = {
    {"full", DRAW_FILL_FULL},
    {"empty", DRAW_FILL_EMPTY}
};

std::unordered_map<std::string, DOT_STYLE> dotStyleMap = {
    {"around", DOT_FILL_AROUND},
    {"rightup", DOT_FILL_RIGHTUP}
};


void execute_command(const PaperCommand& command) {
    auto it = cmdMap.find(command.name);

    if (it != cmdMap.end()) {
        it->second(command);
    } else {
        log(WARNING, ("Unknown command: " + command.name + "\n").c_str());
    }
}
