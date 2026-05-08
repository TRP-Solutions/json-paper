#ifndef E_PAPER_EMULATOR_INPUT_FIELD_H
#define E_PAPER_EMULATOR_INPUT_FIELD_H

#include "raylib.h"
#include <string>
#include <vector>

#define INPUT_FIELD_MAX_LENGTH 64
#define INPUT_FIELD_MAX_HISTORY 50

class InputField {
public:
    Rectangle bounds;
    int fontSize = 0;
    bool active = false;

    int x;
    int y;
    int height;
    int width;


    char text[INPUT_FIELD_MAX_LENGTH] = {0};
    int length = 0;

    InputField(int x, int y, int width, int height, int size);

    void Update();
    void Draw();
    void Clear();

    bool WasEnterPressed() const { return enterPressed; }
    bool IsTextMarked() const { return selectionStart != selectionEnd; }

    void AddToHistory(const char* entry);
    void ClearHistory();

private:
    // Cursor
    int cursorPos = 0;
    double cursorBlinkTimer = 0.0;
    bool cursorVisible = true;
    double cursorBlinkSpeed = 0.5;

    // Selection
    int selectionStart = 0;
    int selectionEnd = 0;

    // Key repeat
    double repeatDelay = 0.30;
    double repeatRate = 0.05;
    double backspaceStart = 0.0;
    double lastBackspaceRepeat = 0.0;
    double arrowStart = 0.0;
    double lastArrowRepeat = 0.0;

    // History
    std::vector<std::string> history;
    int historyOffset = 0;

    // State
    bool enterPressed = false;

    // Helper functions
    void HandleCharInput(int key);
    void HandleBackspace();
    void HandleDelete();
    void HandleLeftArrow();
    void HandleRightArrow();
    void HandleCtrlA();
    void HandleCtrlC();
    void HandleCtrlV();
    void HandleEnter();
    void HandleHistoryUp();
    void HandleHistoryDown();

    void DeleteSelection();
    void ReplaceSelection(const char* newText);
    int GetTextWidth(const char* str) const;
    int GetTextWidthUpTo(int charCount) const;
    bool IsCtrlDown() const;
};

#endif