
//
// Created by mikke on 31/03/2026.
//

#ifndef GUI_RAYLIB_H
#define GUI_RAYLIB_H

#include <cstdint>

class GUI_Raylib {
public:
    static void DrawEPD();
    static int get_pixel(int x, int y);

    // New: Copy image data to internal display buffer (simulates e-paper display RAM)
    static void CopyToDisplayBuffer(const unsigned char* image, int width, int height, int widthByte);
    static void ClearDisplayBuffer(int color);

private:
    static unsigned char* displayBuffer;  // Simulates e-paper's internal memory
    static int displayWidth;
    static int displayHeight;
    static int displayWidthByte;
};

#endif //GUI_RAYLIB_H