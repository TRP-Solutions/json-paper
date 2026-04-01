#ifndef E_PAPER_EMULATOR_INPUT_FIELD_H
#define E_PAPER_EMULATOR_INPUT_FIELD_H

#include "raylib.h"

#define INPUT_FIELD_MAX_LENGTH 64

class InputField {
public:
    Rectangle bounds;

    char text[INPUT_FIELD_MAX_LENGTH] = {0};
    int length = 0;
    int fontSize = 0;
    bool active = false;

    InputField(int x, int y, int width, int height, int size);

    void Update();
    void Draw();
};

#endif