#include "../../Arduino/src/debug_custom/debug.h"

#include "manager/console_manager.h"

void log(LogLevel level, const char* msg) {
    ConsoleManager::get().log(level, msg);
}
