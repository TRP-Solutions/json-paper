#include "simulate.h"

#include "canvas/gui_paint.h"
#include "config/dev_config.h"
#include "core/core.h"
#include "debug/debug.h"
#include "e-Paper/epd_5in79g.h"
#include "gui/gui_raylib.h"

int SimulatePaperDislay(std::string link) {
    uint8_t *BlackImage;
    UWORD Imagesize = ((EPD_5in79G_WIDTH + 3) / 4) * EPD_5in79G_HEIGHT;

    if((BlackImage = (uint8_t *)malloc(Imagesize)) == NULL) {
        log(INFO, "Failed to apply for black memory...\r\n");
        return -1;
    }

    Paint_NewImage(BlackImage, EPD_5in79G_WIDTH/2, EPD_5in79G_HEIGHT/2, 0, WHITE);
    Paint_SetScale(4);

    Paint_SelectImage(BlackImage);
    Paint_Clear(0xFF);

    log(INFO, "SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_5in79G_WHITE);

    // 2.Drawing on the image
    log(INFO, "Drawing:BlackImage\r\n");

    draw_epd_5in79g_remote(link);

    log(INFO, "EPD_Display\r\n");
    GUI_Raylib::CopyToDisplayBuffer(BlackImage, EPD_5in79G_WIDTH/2, EPD_5in79G_HEIGHT/2,
                             (EPD_5in79G_WIDTH/2 + 3) / 4);

    free(BlackImage);
    BlackImage = NULL;

    return 0;
}
