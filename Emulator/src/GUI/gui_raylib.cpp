//
// Created by mikke on 31/03/2026.
//

#include "GUI_Raylib.h"

#include "raylib.h"
#include <cstring>

// Static member definitions - this is the emulator's "display RAM"
unsigned char* GUI_Raylib::displayBuffer = nullptr;
int GUI_Raylib::displayWidth = 0;
int GUI_Raylib::displayHeight = 0;
int GUI_Raylib::displayWidthByte = 0;

void GUI_Raylib::CopyToDisplayBuffer(const unsigned char* image, int width, int height, int widthByte)
{
    // Allocate or reallocate if dimensions changed
    int bufferSize = widthByte * height;
    if (displayBuffer == nullptr || displayWidth != width || displayHeight != height) {
        delete[] displayBuffer;
        displayBuffer = new unsigned char[bufferSize];
        displayWidth = width;
        displayHeight = height;
        displayWidthByte = widthByte;
    }

    // Copy image to display buffer (simulates sending data to e-paper)
    memcpy(displayBuffer, image, bufferSize);
}

void GUI_Raylib::ClearDisplayBuffer(int color)
{
    if (displayBuffer == nullptr) return;

    // For scale 4: each byte holds 4 pixels (2 bits each)
    unsigned char fillByte = (color << 6) | (color << 4) | (color << 2) | color;
    memset(displayBuffer, fillByte, displayWidthByte * displayHeight);
}

int GUI_Raylib::get_pixel(int x, int y)
{
    if (displayBuffer == nullptr) return 1; // WHITE
    if (x >= displayWidth || y >= displayHeight)
        return 1;

    int addr = (x / 4) + y * displayWidthByte;
    unsigned char byte = displayBuffer[addr] & 0xFF;

    int shift = (3 - (x % 4)) * 2;
    return (byte >> shift) & 0x03;
}

int scale = 4;
void GUI_Raylib::DrawEPD()
{
    if (displayBuffer == nullptr) return;

    for (int y = 0; y < displayHeight; y++)
    {
        for (int x = 0; x < displayWidth; x++)
        {
            int c = GUI_Raylib::get_pixel(x, y);

            Color color;

            switch (c)
            {
                case 0: color = BLACK; break;
                case 1: color = WHITE; break;
                case 2: color = YELLOW; break;
                case 3: color = RED; break;
                default: color = MAGENTA; break;
            }

            DrawRectangle(x * scale, y * scale, scale, scale, color);
        }
    }
}