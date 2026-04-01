#include "input_field.h"
#include <cstring>

InputField::InputField(int x, int y, int width, int height, int size)
{
    bounds = { (float)x, (float)y, (float)width, (float)height };
    fontSize = size;
}

void InputField::Update()
{
    Vector2 mouse = GetMousePosition();

    // Focus handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        active = CheckCollisionPointRec(mouse, bounds);
    }

    if (active)
    {
        int key = GetCharPressed();

        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (length < INPUT_FIELD_MAX_LENGTH - 1))
            {
                text[length++] = (char)key;
                text[length] = '\0';
            }
            key = GetCharPressed();
        }

        // Backspace
        if (IsKeyPressed(KEY_BACKSPACE) && length > 0)
        {
            length--;
            text[length] = '\0';
        }
    }
}

void InputField::Draw()
{
    // Background
    DrawRectangleRec(bounds, LIGHTGRAY);

    // Border (red if active)
    DrawRectangleLinesEx(bounds, 2, active ? RED : DARKGRAY);

    // Text
    DrawText(text, bounds.x + 5, bounds.y + (bounds.height - fontSize) / 2, fontSize, BLACK);

    // Cursor
    if (active)
    {
        int textWidth = MeasureText(text, fontSize);

        DrawLine(
            bounds.x + 5 + textWidth,
            bounds.y + 5,
            bounds.x + 5 + textWidth,
            bounds.y + bounds.height - 5,
            BLACK
        );
    }
}