#ifndef E_PAPER_EMULATOR_DEBUG_H
#define E_PAPER_EMULATOR_DEBUG_H
#include <string>

enum LogLevel {
    FATAL,
    WARNING,
    INFO,
    SUCCESS
};

void log(LogLevel level, const char* msg);

#endif //E_PAPER_EMULATOR_DEBUG_H