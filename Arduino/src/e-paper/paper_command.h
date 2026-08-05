//
// Created by mikke on 01/04/2026.
//

#ifndef E_PAPER_EMULATOR_COMMAND_H
#define E_PAPER_EMULATOR_COMMAND_H
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "epd_5in79g.h"
#include "../canvas/gui_paint.h"

struct PaperTextSpan {
    std::string text;
    std::string family = "sans";
    std::string weight = "regular";
    int size = 16;
    std::string color = "black";
    int letterSpacing = 0;
    bool underline = false;
    bool strikeout = false;
};

struct PaperTextCommand {
    int x = 0, y = 0, width = 0, height = 0, lineSpacing = 0;
    std::string background = "transparent";
    std::string horizontalAlign = "left";
    std::string verticalAlign = "top";
    std::string wrap = "word";
    std::string overflow = "ellipsis";
    std::vector<PaperTextSpan> spans;
};

struct PaperCommand {
    std::string name;
    std::map<std::string, std::string> args;
    PaperTextCommand text;
};

using CmdHandler = std::function<void(const PaperCommand&)>;

void execute_command(const PaperCommand& command);

extern std::unordered_map<std::string, CmdHandler> cmdMap;
extern std::unordered_map<std::string, uint8_t> colorMap;
extern std::unordered_map<std::string, LINE_STYLE> lineStyleMap;
extern std::unordered_map<std::string, DRAW_FILL> drawFillMap;
extern std::unordered_map<std::string, DOT_STYLE> dotStyleMap;

#endif //E_PAPER_EMULATOR_COMMAND_H
