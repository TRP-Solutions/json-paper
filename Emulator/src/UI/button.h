//
// Created by mikke on 31/03/2026.
//

#ifndef E_PAPER_EMULATOR_BUTTON_H
#define E_PAPER_EMULATOR_BUTTON_H
#include <functional>
#include <string>

#include "raylib.h"

class Button {
public:
    Rectangle bounds;
    std::string text;
    int fontSize;

    bool hovered = false;
    bool pressed = false;

    std::function<void()> onClick;

    Button(int x, int y, int w, int h, const std::string& label, int fontSize);

    void Update();
    void Draw();
};

#endif //E_PAPER_EMULATOR_BUTTON_H