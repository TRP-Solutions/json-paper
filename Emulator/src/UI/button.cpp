//
// Created by mikke on 31/03/2026.
//

#include "button.h"

#include "raylib.h"

Button::Button(int x, int y, int w, int h, const std::string& label, int fontSize)
{
    bounds = { (float)x, (float)y, (float)w, (float)h };
    text = label;
    this->fontSize = fontSize;
}

void Button::Update()
{
    Vector2 mouse = GetMousePosition();

    hovered = CheckCollisionPointRec(mouse, bounds);

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        pressed = true;

        if (onClick)
            onClick();
    }
    else
    {
        pressed = false;
    }
}

void Button::Draw()
{
    Color color = DARKGRAY;

    if (hovered) color = GRAY;
    if (pressed) color = DARKBLUE;

    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 2, BLACK);

    int textWidth = MeasureText(text.c_str(), fontSize);

    DrawText(
        text.c_str(),
        bounds.x + (bounds.width - textWidth) / 2,
        bounds.y + (bounds.height - fontSize) / 2,
        fontSize,
        WHITE
    );
}