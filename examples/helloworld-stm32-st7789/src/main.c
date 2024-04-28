/*
 * MCU renderer
 * 320x240 ST7789 on STM32 Blue Pill demo
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include <stdint.h>
#include <string.h>

#include "stm32.h"
#include "st7789.h"

#include "../../../fonts/font_material_symbolsR12_4.h"

#include "../../../fonts/font_robotoR12_4.h"

#include "../../../fonts/font_robotoR24_1.h"
#include "../../../fonts/font_robotoR24_2.h"
#include "../../../fonts/font_robotoR24_3.h"
#include "../../../fonts/font_robotoR24_4.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define STATUS_BAR_HEIGHT 20
#define CONTENT_HEIGHT (DISPLAY_HEIGHT - STATUS_BAR_HEIGHT)
#define CELL_HEIGHT (CONTENT_HEIGHT / 4)

mr_t mr;

const uint8_t *demo_fonts[] = {
    font_robotoR24_1,
    font_robotoR24_2,
    font_robotoR24_3,
    font_robotoR24_4,
};

const char *demo_strings[] = {
    "Hello world! 1 bpp",
    "Hello world! 2 bpp",
    "Hello world! 3 bpp",
    "Hello world! 4 bpp",
};

int main(int argc, char *argv[])
{
    mr_t mr;

    initSystem();
    initST7789(&mr);

    // Draw demo strings

    mr_rectangle_t rectangle;
    mr_point_t offset;

    for (uint32_t i = 0; i < 4; i++)
    {
        mr_set_font(&mr, demo_fonts[i]);
        mr_set_text_color(&mr, mr_get_color(0xf1f5fb));
        mr_set_fill_color(&mr,
                          (i % 2)
                              ? mr_get_color(0x121a24)
                              : mr_get_color(0x1b2633));

        // Center horizontally and vertically
        rectangle = (mr_rectangle_t){0,
                                     STATUS_BAR_HEIGHT + CELL_HEIGHT * i,
                                     DISPLAY_WIDTH,
                                     CELL_HEIGHT};
        offset = (mr_point_t){
            (DISPLAY_WIDTH - mr_get_utf8_text_width(&mr, demo_strings[i])) / 2,
            (CELL_HEIGHT - mr_get_line_height(&mr)) / 2};

        mr_draw_utf8_text(&mr,
                          demo_strings[i],
                          &rectangle,
                          &offset);
    }

    // Draw status bar

    mr_set_text_color(&mr, mr_get_color(0xffffff));
    mr_set_fill_color(&mr, mr_get_color(0x000000));

    // Draw time of status bar

    mr_set_font(&mr, font_robotoR12_4);
    char string[16] = "12:34";

    rectangle = (mr_rectangle_t){0,
                                 0,
                                 DISPLAY_WIDTH / 2,
                                 STATUS_BAR_HEIGHT};
    offset = (mr_point_t){
        FONT_ROBOTOR12_4_CAP_HEIGHT / 2,
        (STATUS_BAR_HEIGHT - mr_get_line_height(&mr)) / 2};

    mr_draw_utf8_text(&mr,
                      string,
                      &rectangle,
                      &offset);

    // Draw icons of status bar

    mr_set_font(&mr, font_material_symbolsR12_4);

    strcpy(string, "\xee\x86\xa7\xee\x86\xa4");

    rectangle = (mr_rectangle_t){DISPLAY_WIDTH / 2,
                                 0,
                                 DISPLAY_WIDTH / 2,
                                 STATUS_BAR_HEIGHT};

    offset = (mr_point_t){
        DISPLAY_WIDTH / 2 - mr_get_utf8_text_width(&mr, string) -
            FONT_ROBOTOR12_4_CAP_HEIGHT / 2,
        (STATUS_BAR_HEIGHT - mr_get_line_height(&mr)) / 2};

    mr_draw_utf8_text(&mr,
                      string,
                      &rectangle,
                      &offset);
}
