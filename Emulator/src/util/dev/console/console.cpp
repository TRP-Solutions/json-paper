#include "util/dev/console/console.h"

#include <cstring>

#include "font_data.h"
#include "raylib.h"
#include "util/dev/console/command/registry.h"
#include "util/dev/console/command/commands/core_command.h"

Console::Console() {
    RegisterCoreCommands(mRegistry);

    mFont = LoadFontFromMemory(
    ".ttf",
    VictorMono_Medium,
    VictorMono_Medium_len,
    14,
    nullptr,
    0
);
}

void Console::setOpen(bool open) {
    mOpen = open;
}

bool Console::isOpen() const {
    return mOpen;
}

void Console::draw()
{
    const int padding = 8;
    const int fontSize = 14;
    const int lineHeight = fontSize + 2;
    const int height = 300;

    DrawRectangle(0, 0, GetScreenWidth(), height, Color{ 0, 0, 0, 220 });
    const int inputBarHeight = lineHeight + padding * 2;

    DrawRectangle(0, height - inputBarHeight, GetScreenWidth(), inputBarHeight, Color{ 35, 35, 35, 240 });

    DrawLine(0, height - inputBarHeight, GetScreenWidth(), height - inputBarHeight, Color{ 60, 60, 60, 255 });

    const int logAreaHeight = height - inputBarHeight;
    int maxVisibleLines = (logAreaHeight - padding * 2) / lineHeight;

    // clamp scroll offset
    if (mScrollOffset < 0)
        mScrollOffset = 0;

    int maxScroll =
        mLogs.size() > maxVisibleLines
        ? mLogs.size() - maxVisibleLines
        : 0;

    if (mScrollOffset > maxScroll)
        mScrollOffset = maxScroll;

    if (maxVisibleLines > mLogs.size())
        maxVisibleLines = mLogs.size();

    int start = mLogs.size() - 1 - mScrollOffset;

    // draw logs
    for (int i = 0; i < maxVisibleLines; i++)
    {
        int logIndex = start - i;
        if (logIndex < 0)
            break;

        Color color = RAYWHITE;
        switch (mLogs[logIndex].level)
        {
            case FATAL:   color = RED;    break;
            case WARNING: color = YELLOW; break;
            case INFO:    color = RAYWHITE; break;
            case SUCCESS: color = GREEN;  break;
            case ESP: color = BLUE;  break;
        }

        DrawTextEx(
            mFont,
            mLogs[logIndex].text.c_str(),
            {
                static_cast<float>(padding),
                static_cast<float>(logAreaHeight - padding - lineHeight * (i + 1))
            },
            static_cast<float>(fontSize),
            1.0f,
            color
        );
    }

    // input draw thing
    const float inputY = static_cast<float>(height - padding - lineHeight);

    DrawTextEx(
        mFont,
        ">",
        { static_cast<float>(padding), inputY },
        static_cast<float>(fontSize),
        1.0f,
        RAYWHITE
    );

    DrawTextEx(
        mFont,
        mInput.c_str(),
        { static_cast<float>(padding + 10), inputY },
        static_cast<float>(fontSize),
        1.0f,
        RAYWHITE
    );

    if (!mCursorBlink || static_cast<int>(GetTime() * 2) % 2 == 0)
    {
        if (mCursorPos < 0) mCursorPos = 0;

        std::string temp = mInput.substr(0, mCursorPos);

        Vector2 textSize = MeasureTextEx(
            mFont,
            temp.c_str(),
            static_cast<float>(fontSize),
            1.0f
        );

        const float textStartX = static_cast<float>(padding + 11);
        const float cursorX = textStartX + textSize.x;

        const float cursorTopY = inputY;
        const float cursorBottomY = inputY + fontSize;

        DrawLine(
            static_cast<int>(cursorX),
            static_cast<int>(cursorTopY),
            static_cast<int>(cursorX),
            static_cast<int>(cursorBottomY),
            RAYWHITE
        );
    }
}

void Console::log(LogLevel level, const char* format, ...)
{
    char buffer[512];

    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (mLogs.size() >= CONSOLE_MAX_LOG) {
        mLogs.pop_front();
    }

    mLogs.push_back(CommandLine{
        level,
        std::string(buffer)
    });
}

void Console::autoComplete() {
    if (mInput.empty()) return;

    if (mCursorPos > mInput.length()) mCursorPos = mInput.length();

    size_t wordStart = mCursorPos;
    bool inQuotes = false;

    // Scan left
    for (size_t i = mCursorPos; i > 0; i--)
    {
        if (mInput[i - 1] == '"')
            inQuotes = !inQuotes;

        if (!inQuotes && mInput[i - 1] == ' ')
        {
            wordStart = i;
            break;
        }

        wordStart = i - 1;
    }

    size_t wordEnd = mCursorPos;
    inQuotes = false;

    // Scan right
    for (size_t i = mCursorPos; i < mInput.length(); i++)
    {
        if (mInput[i] == '"') inQuotes = !inQuotes;

        if (!inQuotes && mInput[i] == ' ')
        {
            wordEnd = i;
            break;
        }

        wordEnd = i + 1;
    }

    if (mCursorPos != wordEnd) return;

    std::string prefix = mInput.substr(wordStart, mCursorPos - wordStart);

    auto tokens = SplitArgs(mInput);

    if (tokens.empty()) return;

    if (wordStart == 0)
    {
        for (const auto& [name, cmd] : mRegistry.all())
        {
            if (!name.starts_with(prefix)) continue;
            std::string newInput;

            newInput += name;
            newInput += mInput.substr(wordEnd);

            mInput = std::move(newInput);
            mCursorPos = name.size();

            return;
        }

        return;
    }

    const std::string& cmdName = tokens[0];

    Command* cmd = mRegistry.find(cmdName);
    if (!cmd)return;

    int argIndex = 0;
    int count = 0;
    inQuotes = false;

    for (int i = 0; i < wordStart; ++i)
    {
        if (mInput[i] == '"')
            inQuotes = !inQuotes;

        if (!inQuotes && mInput[i] == ' ')
            count++;
    }
    if (count == 0) return;

    argIndex = count - 1;

    if (argIndex >= cmd->args.size()) return;

    const CommandArg& arg = cmd->args[argIndex];

    if (!arg.completer) return;

    auto suggestions = arg.completer(prefix);

    if (suggestions.empty()) return;

    const std::string& completion = suggestions[0];

    std::string newInput;

    newInput += mInput.substr(0, wordStart);
    newInput += completion;
    newInput += mInput.substr(wordEnd);

    mInput = std::move(newInput);
    mCursorPos = wordStart + completion.size();
}

void Console::handleInput()
{
    static double backspaceStart = 0.0;
    static double lastRepeat = 0.0;

    static double arrowStart = 0.0;
    static double lastArrowRepeat = 0.0;

    const double repeatDelay = 0.30;
    const double repeatRate = 0.05;

    double now = GetTime();

    bool userEditing = false;

    // Character input
    int key = GetCharPressed();

    while (key > 0)
    {
        if (key >= 32 && key <= 126)
        {
            // If text is marked, delete it first
            if (mSelectionStart != mSelectionEnd)
            {
                size_t start = std::min(mSelectionStart, mSelectionEnd);
                size_t end = std::max(mSelectionStart, mSelectionEnd);
                mInput.erase(start, end - start);
                mCursorPos = start;
                mSelectionStart = mCursorPos;
                mSelectionEnd = mCursorPos;
            }

            if (mInput.size() < CONSOLE_MAX_INPUT - 1)
            {
                userEditing = true;

                mInput.insert(
                    mCursorPos,
                    1,
                    static_cast<char>(key)
                );

                ++mCursorPos;
                mSelectionStart = mCursorPos;
                mSelectionEnd = mCursorPos;
            }
        }

        key = GetCharPressed();
    }

    // Control key state
    bool ctrlDown =
        IsKeyDown(KEY_LEFT_CONTROL) ||
        IsKeyDown(KEY_RIGHT_CONTROL);

    // Ctrl+A: Select all
    if (IsKeyPressed(KEY_A) && ctrlDown)
    {
        mSelectionStart = 0;
        mSelectionEnd = mInput.size();
    }

    // Ctrl+C: Copy (marked text or all)
    if (IsKeyPressed(KEY_C) && ctrlDown)
    {
        std::string textToCopy;

        if (mSelectionStart != mSelectionEnd)
        {
            size_t start = std::min(mSelectionStart, mSelectionEnd);
            size_t end = std::max(mSelectionStart, mSelectionEnd);
            textToCopy = mInput.substr(start, end - start);
        }
        else if (!mInput.empty())
        {
            textToCopy = mInput;
        }

        if (!textToCopy.empty())
        {
            SetClipboardText(textToCopy.c_str());
        }
    }

    // Ctrl+V: Paste
    if (IsKeyPressed(KEY_V) && ctrlDown)
    {
        const char* clipboardText = GetClipboardText();
        if (clipboardText != nullptr)
        {
            std::string pasteText(clipboardText);

            // Filter out newlines and other control characters
            pasteText.erase(
                std::remove_if(pasteText.begin(), pasteText.end(),
                    [](unsigned char c) { return c < 32 && c != '\t'; }),
                pasteText.end()
            );

            // If text is marked, replace it
            if (mSelectionStart != mSelectionEnd)
            {
                size_t start = std::min(mSelectionStart, mSelectionEnd);
                size_t end = std::max(mSelectionStart, mSelectionEnd);
                size_t deleteLen = end - start;

                mInput.erase(start, deleteLen);
                mCursorPos = start;
                mSelectionStart = start;
                mSelectionEnd = start;
            }

            size_t spaceLeft = CONSOLE_MAX_INPUT - 1 - mInput.size();
            size_t insertLen = std::min(pasteText.size(), spaceLeft);

            if (insertLen > 0)
            {
                mInput.insert(mCursorPos, pasteText.substr(0, insertLen));
                mCursorPos += insertLen;
                mSelectionStart = mCursorPos;
                mSelectionEnd = mCursorPos;
                userEditing = true;
            }
        }
    }

    // Backspace
    auto handleBackspace = [&]()
    {
        // If text is marked, delete the marked text
        if (mSelectionStart != mSelectionEnd)
        {
            size_t start = std::min(mSelectionStart, mSelectionEnd);
            size_t end = std::max(mSelectionStart, mSelectionEnd);
            mInput.erase(start, end - start);
            mCursorPos = start;
            mSelectionStart = mCursorPos;
            mSelectionEnd = mCursorPos;
            return;
        }

        if (mCursorPos == 0 || mInput.empty())
            return;

        if (ctrlDown)
        {
            size_t start = mCursorPos;

            // Skip spaces
            while (start > 0 && mInput[start - 1] == ' ')
                --start;

            // Skip word
            bool inQuotes = false;

            for (size_t i = start; i > 0; --i)
            {
                if (mInput[i - 1] == '"')
                    inQuotes = !inQuotes;

                if (!inQuotes && mInput[i - 1] == ' ')
                {
                    start = i;
                    break;
                }

                start = i - 1;
            }

            mInput.erase(start, mCursorPos - start);
            mCursorPos = start;
        }
        else
        {
            mInput.erase(mCursorPos - 1, 1);
            --mCursorPos;
        }

        mSelectionStart = mCursorPos;
        mSelectionEnd = mCursorPos;
    };

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        backspaceStart = now;
        lastRepeat = now;

        handleBackspace();
    }
    else if (IsKeyDown(KEY_BACKSPACE))
    {
        if (now - backspaceStart >= repeatDelay &&
            now - lastRepeat >= repeatRate)
        {
            lastRepeat = now;

            handleBackspace();
        }
    }

    // Enter
    if (IsKeyPressed(KEY_ENTER))
    {
        if (!mInput.empty())
        {
            log(INFO, "> %s", mInput.c_str());
            executeCommand();

            mHistory.push_back(mInput);

            if (mHistory.size() > CONSOLE_MAX_HISTORY)
                mHistory.erase(mHistory.begin());

            mInput.clear();
            mCursorPos = 0;
            mSelectionStart = 0;
            mSelectionEnd = 0;
            mHistoryOffset = 0;
        }
    }

    // History Up
    if (IsKeyPressed(KEY_UP))
    {
        if (!mHistory.empty() &&
            mHistoryOffset < mHistory.size())
        {
            ++mHistoryOffset;

            size_t index =
                mHistory.size() - mHistoryOffset;

            mInput = mHistory[index];
            mCursorPos = mInput.size();
            mSelectionStart = mCursorPos;
            mSelectionEnd = mCursorPos;
        }
    }

    // History Down
    if (IsKeyPressed(KEY_DOWN))
    {
        if (mHistoryOffset > 1)
        {
            --mHistoryOffset;

            size_t index =
                mHistory.size() - mHistoryOffset;

            mInput = mHistory[index];
            mCursorPos = mInput.size();
            mSelectionStart = mCursorPos;
            mSelectionEnd = mCursorPos;
        }
        else
        {
            mHistoryOffset = 0;
            mInput.clear();
            mCursorPos = 0;
            mSelectionStart = 0;
            mSelectionEnd = 0;
        }
    }

    // Left Arrow
    if (IsKeyPressed(KEY_LEFT))
    {
        arrowStart = now;
        lastArrowRepeat = now;

        if (mCursorPos > 0)
        {
            --mCursorPos;
        }

        // Clear selection
        mSelectionStart = mCursorPos;
        mSelectionEnd = mCursorPos;
    }
    else if (IsKeyDown(KEY_LEFT))
    {
        if (now - arrowStart >= repeatDelay &&
            now - lastArrowRepeat >= repeatRate)
        {
            lastArrowRepeat = now;

            if (mCursorPos > 0)
            {
                --mCursorPos;
            }

            // Clear selection
            mSelectionStart = mCursorPos;
            mSelectionEnd = mCursorPos;
        }
    }

    // Right Arrow
    if (IsKeyPressed(KEY_RIGHT))
    {
        arrowStart = now;
        lastArrowRepeat = now;

        if (mCursorPos < mInput.size())
        {
            ++mCursorPos;
        }

        // Clear selection
        mSelectionStart = mCursorPos;
        mSelectionEnd = mCursorPos;
    }
    else if (IsKeyDown(KEY_RIGHT))
    {
        if (now - arrowStart >= repeatDelay &&
            now - lastArrowRepeat >= repeatRate)
        {
            lastArrowRepeat = now;

            if (mCursorPos < mInput.size())
            {
                ++mCursorPos;
            }

            // Clear selection
            mSelectionStart = mCursorPos;
            mSelectionEnd = mCursorPos;
        }
    }

    // Cursor blink - don't blink if text is marked
    bool isTextMarked = (mSelectionStart != mSelectionEnd);

    bool arrowActive =
        IsKeyDown(KEY_LEFT) ||
        IsKeyDown(KEY_RIGHT);

    bool backspaceActive =
        IsKeyDown(KEY_BACKSPACE);

    mCursorBlink =
        !(isTextMarked || arrowActive || backspaceActive || userEditing);

    // Autocomplete
    if (IsKeyPressed(KEY_TAB))
    {
        // Clear selection when autocompleting
        mSelectionStart = mCursorPos;
        mSelectionEnd = mCursorPos;
        autoComplete();
    }

    // Scroll
    float wheel = GetMouseWheelMove();

    if (wheel > 0)
        ++mScrollOffset;
    else if (wheel < 0)
        --mScrollOffset;
}


Console::~Console() {
}

void Console::executeCommand()
{
    if (mInput.empty())
        return;

    // Reset cursor
    mCursorPos = 0;

    size_t spacePos = mInput.find(' ');

    std::string commandName;
    std::string argString;

    if (spacePos != std::string::npos)
    {
        commandName = mInput.substr(0, spacePos);
        argString   = mInput.substr(spacePos + 1);
    }
    else
    {
        commandName = mInput;
        argString.clear();
    }

    // Find command
    Command* command = mRegistry.find(commandName);

    if (!command)
    {
        log(FATAL,
            "Unknown command. Type 'help' to see command list");
        return;
    }

    // Tokenize args
    auto tokens = SplitArgs(argString);

    ParsedArgs parsed;

    if (!ParseArgs(*command, tokens, parsed))
    {
        log(FATAL,
            "Invalid arguments. Type 'help %s' for usage",
            commandName.c_str());
        return;
    }

    command->execute(parsed);
}


void Console::clearLogs() {
    mLogs.clear();
    mScrollOffset = 0;
}

