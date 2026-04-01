/*****************************************************************************
* | File      	:	  EPD_Test.h
* | Author      :   Waveshare team
* | Function    :   e-Paper test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-11
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef _EPD_TEST_H_
#define _EPD_TEST_H_

//#include "ImageData.h"
#include <stdlib.h> // malloc() free()
#include "debug.h"
#include "Canvas/GUI_Paint.h"
#include "Font/fonts.h"

class command;

int EPD_5in79g_test(void);
void draw_epd_5in79g_remote(std::string link);
int simulate_epd_5in79g_test(std::string link);

// Core
void Clear(const command& command);
void ClearWindow(const command& command);

// Display control
void SetRotate(const command& command);
void SetMirroring(const command& command);
void SetPixel(const command& command);

// Drawing
void DrawPoint(const command& command);
void DrawLine(const command& command);
void DrawRectangle(const command& command);
void DrawCircle(const command& command);

// Text (high-level)
void DrawString(const command& command);

// Text (low-level primitives)
void DrawChar(const command& command);
void DrawNum(const command& command);
void DrawTime(const command& command);
#endif
