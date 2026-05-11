#include "core/core.h"
#include "simulate/simulate.h"
#undef WHITE
#undef BLACK
#undef RED

#include "raylib.h"
#include "gui/gui_raylib.h"
#include "manager/console_manager.h"
#include "ui/button.h"
#include "ui/input_field.h"

int main(void)
{
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] emulation - basic window");

    SetTargetFPS(60);

    std::string lastExecutedText;
    bool hasExecuted = false;
    bool wasFocused = true;

    ConsoleManager::create();

    Button button(20, 570, 300, 50, "Test", 30);
    InputField input_field(340, 570, 700, 50, 30);

    button.onClick = [&input_field, &lastExecutedText, &hasExecuted]() {
        SimulatePaperDislay(input_field.text);
        lastExecutedText = std::string(input_field.text);
        hasExecuted = true;
    };
    while (!WindowShouldClose())
    {
        bool isFocused = IsWindowFocused();

        bool regainedFocus = isFocused && !wasFocused;

        bool textChanged = std::string(input_field.text) != lastExecutedText;

        if (hasExecuted && !textChanged && (regainedFocus)) {
            SimulatePaperDislay(input_field.text);
            lastExecutedText = std::string(input_field.text);
        }

        wasFocused = isFocused;

        if (IsKeyPressed(KEY_F1)) {
            if (ConsoleManager::has()) {
                ConsoleManager::get().setOpen(!ConsoleManager::get().isOpen());
            }
        }

        if (ConsoleManager::has() && ConsoleManager::get().isOpen()) ConsoleManager::get().handleInput();

        button.Update();
        input_field.Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        button.Draw();
        input_field.Draw();
        GUI_Raylib::DrawEPD();

        if (ConsoleManager::has() && ConsoleManager::get().isOpen()) {
            ConsoleManager::get().draw();
        }

        EndDrawing();
    }
    ConsoleManager::destroy();

    CloseWindow();

    return 0;
}


