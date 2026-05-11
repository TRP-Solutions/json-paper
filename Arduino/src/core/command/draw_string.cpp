#include "../../canvas/gui_paint.h"
#include "../../config/dev_config.h"
#include "../core.h"
#include "../../font/fonts.h"

struct PaperCommand;

void DrawChar_Transparent(UWORD Xstart, UWORD Ystart, const char Acsii_Char,
                          sFONT* Font, UWORD Color_Foreground)
{
    UWORD Page, Column;

    const uint8_t *ptr = &Font->table[(Acsii_Char - ' ') * Font->Height * ((Font->Width + 7) / 8)];

    for (Page = 0; Page < Font->Height; Page++) {
        for (Column = 0; Column < Font->Width; Column++) {

            if (ptr[Column / 8] & (0x80 >> (Column % 8))) {
                Paint_SetPixel(Xstart + Column, Ystart + Page, Color_Foreground);
            }

        }
        ptr += (Font->Width + 7) / 8;
    }
}

void DrawString_EN_Transparent(UWORD Xstart, UWORD Ystart, const char *pString,
                                     sFONT* Font, UWORD Color_Foreground)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height) {
        log(INFO, "Paint_DrawString_EN_Transparent out of range\r\n");
        return;
    }

    while (*pString != '\0') {

        if ((Xpoint + Font->Width) > Paint.Width) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        if ((Ypoint + Font->Height) > Paint.Height) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }

        // Draw character WITHOUT background
        DrawChar_Transparent(Xpoint, Ypoint, *pString, Font, Color_Foreground);

        pString++;
        Xpoint += Font->Width;
    }
}

void DrawString(const PaperCommand& command) {
    std::string xs, ys, text, fg, bg, fs;

    if (!GetRequiredArg(command, "x", xs) ||
        !GetRequiredArg(command, "y", ys) ||
        !GetRequiredArg(command, "text", text) ||
        !GetRequiredArg(command, "foreground", fg) ||
        !GetRequiredArg(command, "background", bg) ||
        !GetRequiredArg(command, "font", fs))  {
            log(WARNING, "Missing required args for 'draw_string'");

            return;
        }

    int x, y;
    if (!ParseInt(xs, x) || !ParseInt(ys, y)) return;

    uint8_t fg_c;
    sFONT* font;

    if (!ParseColor(fg, fg_c) ||
        !ParseFont(fs, font)) return;

    if (x < 0 || y < 0 ||
    x + font->Width >= Paint.Width ||
    y + font->Height >= Paint.Height) {
        log(
            WARNING,
            "Input exceeds the normal display range for 'draw_string'"
        );

        return;
    }

    if (bg == "transparent") {
        DrawString_EN_Transparent(x, y, text.c_str(), font, fg_c);
        return;
    }

    uint8_t bg_c;
    if (!ParseColor(bg, bg_c)) return;

    Paint_DrawString_EN(x+1, y+1, text.c_str(), font, fg_c, bg_c);
}
