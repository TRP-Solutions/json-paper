#include "input_field.h"
#include <cstring>
#include <algorithm>

InputField::InputField(int x, int y, int width, int height, int size)
{
    bounds = { (float)x, (float)y, (float)width, (float)height };
    fontSize = size;
}

bool InputField::IsCtrlDown() const
{
    return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
}

int InputField::GetTextWidth(const char* str) const
{
    return MeasureText(str, fontSize);
}

int InputField::GetTextWidthUpTo(int charCount) const
{
    if (charCount <= 0) return 0;

    char tempStr[INPUT_FIELD_MAX_LENGTH] = {0};
    strncpy(tempStr, text, charCount);
    return GetTextWidth(tempStr);
}

void InputField::DeleteSelection()
{
    if (selectionStart != selectionEnd)
    {
        int start = std::min(selectionStart, selectionEnd);
        int end = std::max(selectionStart, selectionEnd);

        // Shift remaining text
        memmove(&text[start], &text[end], length - end + 1);
        length -= (end - start);

        cursorPos = start;
        selectionStart = cursorPos;
        selectionEnd = cursorPos;
    }
}

void InputField::ReplaceSelection(const char* newText)
{
    DeleteSelection();

    // Filter out control characters
    char filteredText[INPUT_FIELD_MAX_LENGTH] = {0};
    int filteredLen = 0;

    for (int i = 0; newText[i] != '\0' && filteredLen < INPUT_FIELD_MAX_LENGTH - 1; ++i)
    {
        unsigned char c = (unsigned char)newText[i];
        if (c >= 32 || c == '\t')
        {
            filteredText[filteredLen++] = c;
        }
    }
    filteredText[filteredLen] = '\0';

    int spaceLeft = INPUT_FIELD_MAX_LENGTH - 1 - length;
    int insertLen = std::min(filteredLen, spaceLeft);

    if (insertLen > 0)
    {
        // Shift text to make room
        memmove(&text[cursorPos + insertLen], &text[cursorPos], length - cursorPos + 1);
        strncpy(&text[cursorPos], filteredText, insertLen);

        length += insertLen;
        cursorPos += insertLen;
        selectionStart = cursorPos;
        selectionEnd = cursorPos;
    }
}

void InputField::HandleCharInput(int key)
{
    // If text is marked, delete it first
    if (selectionStart != selectionEnd)
    {
        DeleteSelection();
    }

    if (key >= 32 && key <= 126 && length < INPUT_FIELD_MAX_LENGTH - 1)
    {
        // Shift text to make room
        memmove(&text[cursorPos + 1], &text[cursorPos], length - cursorPos + 1);
        text[cursorPos] = (char)key;
        ++length;
        ++cursorPos;
        selectionStart = cursorPos;
        selectionEnd = cursorPos;
    }
}

void InputField::HandleBackspace()
{
    // If text is marked, delete the marked text
    if (selectionStart != selectionEnd)
    {
        DeleteSelection();
        return;
    }

    if (cursorPos == 0 || length == 0)
        return;

    if (IsCtrlDown())
    {
        int start = cursorPos;

        // Skip spaces
        while (start > 0 && text[start - 1] == ' ')
            --start;

        // Skip word
        bool inQuotes = false;

        for (int i = start; i > 0; --i)
        {
            if (text[i - 1] == '"')
                inQuotes = !inQuotes;

            if (!inQuotes && text[i - 1] == ' ')
            {
                start = i;
                break;
            }

            start = i - 1;
        }

        memmove(&text[start], &text[cursorPos], length - cursorPos + 1);
        length -= (cursorPos - start);
        cursorPos = start;
    }
    else
    {
        memmove(&text[cursorPos - 1], &text[cursorPos], length - cursorPos + 1);
        --length;
        --cursorPos;
    }

    selectionStart = cursorPos;
    selectionEnd = cursorPos;
}

void InputField::HandleDelete()
{
    // If text is marked, delete the marked text
    if (selectionStart != selectionEnd)
    {
        DeleteSelection();
        return;
    }

    if (cursorPos >= length)
        return;

    memmove(&text[cursorPos], &text[cursorPos + 1], length - cursorPos);
    --length;

    selectionStart = cursorPos;
    selectionEnd = cursorPos;
}

void InputField::HandleLeftArrow()
{
    if (cursorPos > 0)
    {
        --cursorPos;
    }

    // Clear selection
    selectionStart = cursorPos;
    selectionEnd = cursorPos;
    cursorBlinkTimer = 0.0;
}

void InputField::HandleRightArrow()
{
    if (cursorPos < length)
    {
        ++cursorPos;
    }

    // Clear selection
    selectionStart = cursorPos;
    selectionEnd = cursorPos;
    cursorBlinkTimer = 0.0;
}

void InputField::HandleCtrlA()
{
    selectionStart = 0;
    selectionEnd = length;
}

void InputField::HandleCtrlC()
{
    char textToCopy[INPUT_FIELD_MAX_LENGTH] = {0};

    if (selectionStart != selectionEnd)
    {
        int start = std::min(selectionStart, selectionEnd);
        int end = std::max(selectionStart, selectionEnd);
        strncpy(textToCopy, &text[start], end - start);
    }
    else if (length > 0)
    {
        strcpy(textToCopy, text);
    }

    if (textToCopy[0] != '\0')
    {
        SetClipboardText(textToCopy);
    }
}

void InputField::HandleCtrlV()
{
    const char* clipboardText = GetClipboardText();
    if (clipboardText != nullptr)
    {
        ReplaceSelection(clipboardText);
    }
}

void InputField::HandleEnter()
{
    if (length > 0)
    {
        enterPressed = true;
        AddToHistory(text);
    }
}

void InputField::HandleHistoryUp()
{
    if (!history.empty() && historyOffset < (int)history.size())
    {
        ++historyOffset;
        int index = history.size() - historyOffset;
        strcpy(text, history[index].c_str());
        length = strlen(text);
        cursorPos = length;
        selectionStart = cursorPos;
        selectionEnd = cursorPos;
    }
}

void InputField::HandleHistoryDown()
{
    if (historyOffset > 1)
    {
        --historyOffset;
        int index = history.size() - historyOffset;
        strcpy(text, history[index].c_str());
        length = strlen(text);
        cursorPos = length;
        selectionStart = cursorPos;
        selectionEnd = cursorPos;
    }
    else
    {
        historyOffset = 0;
        Clear();
    }
}

void InputField::Clear()
{
    memset(text, 0, INPUT_FIELD_MAX_LENGTH);
    length = 0;
    cursorPos = 0;
    selectionStart = 0;
    selectionEnd = 0;
    enterPressed = false;
}

void InputField::AddToHistory(const char* entry)
{
    if (entry != nullptr && entry[0] != '\0')
    {
        history.push_back(std::string(entry));
        if ((int)history.size() > INPUT_FIELD_MAX_HISTORY)
        {
            history.erase(history.begin());
        }
        historyOffset = 0;
    }
}

void InputField::ClearHistory()
{
    history.clear();
    historyOffset = 0;
}

void InputField::Update()
{
    Vector2 mouse = GetMousePosition();
    enterPressed = false;

    // Focus handling
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        active = CheckCollisionPointRec(mouse, bounds);

        if (active)
        {
            // Set cursor position based on click
            float clickX = mouse.x - bounds.x - 5;
            float currentX = 0;

            cursorPos = 0;
            for (int i = 0; i < length; ++i)
            {
                char tempStr[2] = {text[i], '\0'};
                int charWidth = GetTextWidth(tempStr);
                if (currentX + charWidth / 2 > clickX)
                {
                    break;
                }
                currentX += charWidth;
                ++cursorPos;
            }

            selectionStart = cursorPos;
            selectionEnd = cursorPos;
        }
    }

    if (!active)
        return;

    // Update cursor blink
    cursorBlinkTimer += GetFrameTime();
    if (cursorBlinkTimer > cursorBlinkSpeed * 2)
    {
        cursorBlinkTimer = 0.0;
    }

    // Character input
    int key = GetCharPressed();
    while (key > 0)
    {
        HandleCharInput(key);
        key = GetCharPressed();
    }

    // Backspace - with repeat
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        backspaceStart = GetTime();
        lastBackspaceRepeat = GetTime();
        HandleBackspace();
    }
    else if (IsKeyDown(KEY_BACKSPACE))
    {
        double now = GetTime();
        if (now - backspaceStart >= repeatDelay &&
            now - lastBackspaceRepeat >= repeatRate)
        {
            lastBackspaceRepeat = now;
            HandleBackspace();
        }
    }

    // Delete key
    if (IsKeyPressed(KEY_DELETE))
    {
        HandleDelete();
    }

    // Control key combinations
    if (IsKeyPressed(KEY_A) && IsCtrlDown())
    {
        HandleCtrlA();
    }

    if (IsKeyPressed(KEY_C) && IsCtrlDown())
    {
        HandleCtrlC();
    }

    if (IsKeyPressed(KEY_V) && IsCtrlDown())
    {
        HandleCtrlV();
    }

    // Enter
    if (IsKeyPressed(KEY_ENTER))
    {
        HandleEnter();
    }

    // History navigation
    if (IsKeyPressed(KEY_UP))
    {
        HandleHistoryUp();
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        HandleHistoryDown();
    }

    // Arrow keys - with repeat
    if (IsKeyPressed(KEY_LEFT))
    {
        arrowStart = GetTime();
        lastArrowRepeat = GetTime();
        HandleLeftArrow();
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        double now = GetTime();
        if (now - arrowStart >= repeatDelay &&
            now - lastArrowRepeat >= repeatRate)
        {
            lastArrowRepeat = now;
            HandleLeftArrow();
        }
    }

    if (IsKeyPressed(KEY_RIGHT))
    {
        arrowStart = GetTime();
        lastArrowRepeat = GetTime();
        HandleRightArrow();
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        double now = GetTime();
        if (now - arrowStart >= repeatDelay &&
            now - lastArrowRepeat >= repeatRate)
        {
            lastArrowRepeat = now;
            HandleRightArrow();
        }
    }
}

void InputField::Draw()
{
    // Background
    DrawRectangleRec(bounds, LIGHTGRAY);

    // Border
    DrawRectangleLinesEx(bounds, 2, active ? RED : DARKGRAY);

    // Draw selection highlight
    if (selectionStart != selectionEnd)
    {
        int start = std::min(selectionStart, selectionEnd);
        int end = std::max(selectionStart, selectionEnd);

        int selectionStartX = GetTextWidthUpTo(start);
        int selectionEndX = GetTextWidthUpTo(end);

        DrawRectangle(
            bounds.x + 5 + selectionStartX,
            bounds.y + 5,
            selectionEndX - selectionStartX,
            bounds.height - 10,
            Color { 0, 100, 200, 100 }
        );
    }

    // Text
    DrawText(text, bounds.x + 5, bounds.y + (bounds.height - fontSize) / 2, fontSize, BLACK);

    // Cursor
    if (active && selectionStart == selectionEnd)
    {
        bool shouldDrawCursor = (cursorBlinkTimer < cursorBlinkSpeed);
        if (shouldDrawCursor)
        {
            int cursorX = GetTextWidthUpTo(cursorPos);

            DrawLine(
                bounds.x + 5 + cursorX,
                bounds.y + 5,
                bounds.x + 5 + cursorX,
                bounds.y + bounds.height - 5,
                BLACK
            );
        }
    }
}