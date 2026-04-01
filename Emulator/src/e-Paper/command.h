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

#include "e-Paper/EPD_5in79g.h"
#include "Canvas/GUI_Paint.h"

struct command;
using CmdHandler = std::function<void(const command&)>;


struct command {
    std::string name;
    std::map<std::string, std::string> args;
};

void execute_command(const command& command);

extern std::unordered_map<std::string, CmdHandler> cmdMap;
extern std::unordered_map<std::string, uint8_t> colorMap;
extern std::unordered_map<std::string, LINE_STYLE> lineStyleMap;
extern std::unordered_map<std::string, DRAW_FILL> drawFillMap;
extern std::unordered_map<std::string, DOT_STYLE> dotStyleMap;
extern std::unordered_map<std::string, sFONT*> fontMap;

#endif //E_PAPER_EMULATOR_COMMAND_H