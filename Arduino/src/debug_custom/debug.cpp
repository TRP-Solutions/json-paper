#include "debug.h"
#include <Arduino.h>

void log(LogLevel level, const char* msg) {
    Serial.println(msg);
}
