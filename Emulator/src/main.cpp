#include "emulation/EPD_Test.h"
#include "raylib.h"
#include "GUI/gui_raylib.h"
#include "UI/button.h"
#include "UI/input_field.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] emulation - basic window");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    std::string lastExecutedText = "";
    bool hasExecuted = false;
    bool wasFocused = true;

    Button button(20, 550, 300, 50, "Test", 30);
    InputField input_field(340, 550, 700, 50, 30);

    button.onClick = [&input_field, &lastExecutedText, &hasExecuted]() {
        simulate_epd_5in79g_test(input_field.text);
        lastExecutedText = input_field.text;
        hasExecuted = true;
    };
    while (!WindowShouldClose())
    {
        bool isFocused = IsWindowFocused();

        bool regainedFocus = isFocused && !wasFocused;

        bool textChanged = input_field.text != lastExecutedText;

        if (hasExecuted && !textChanged && (regainedFocus)) {
            simulate_epd_5in79g_test(input_field.text);
            lastExecutedText = input_field.text;
        }

        wasFocused = isFocused;

        button.Update();
        input_field.Update();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        button.Draw();
        input_field.Draw();
        GUI_Raylib::DrawEPD();

        EndDrawing();
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}