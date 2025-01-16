/*
 * MCU renderer example
 * Hello world
 *
 * (C) 2023-2025 Gissio
 *
 * License: MIT
 */

#include <stdint.h>
#include <string.h>

#include "display.h"
#include "system.h"

#include "mcu-renderer-fonts/font_material_symbolsR12_4.h"

#include "mcu-renderer-fonts/font_robotoM12_4.h"

#include "mcu-renderer-fonts/font_robotoM48_1.h"
#include "mcu-renderer-fonts/font_robotoM48_2.h"
#include "mcu-renderer-fonts/font_robotoM48_3.h"
#include "mcu-renderer-fonts/font_robotoM48_4.h"

#define STATUSBAR_X 0
#define STATUSBAR_Y 0
#define STATUSBAR_WIDTH DISPLAY_WIDTH
#define STATUSBAR_HEIGHT 40
#define STATUSBAR_ITEMS 4

#define CONTENT_X 0
#define CONTENT_Y STATUSBAR_HEIGHT
#define CONTENT_WIDTH DISPLAY_WIDTH
#define CONTENT_HEIGHT (DISPLAY_HEIGHT - STATUSBAR_HEIGHT)

#define CELL_WIDTH (CONTENT_WIDTH / 2)
#define CELL_HEIGHT (CONTENT_HEIGHT / 2)

int main(void)
{
    mr_t mr;

    init_system();
    init_display(&mr);

    // Draw content
    mr_rectangle_t rectangle;
    mr_point_t offset;

    const char *test_strings[] = {
        "1bit",
        "2bit",
        "3bit",
        "4bit",
    };
    const uint8_t *test_fonts[] = {
        font_robotoM48_1,
        font_robotoM48_2,
        font_robotoM48_3,
        font_robotoM48_4,
    };
    for (uint32_t y = 0; y < 2; y++)
    {
        for (uint32_t x = 0; x < 2; x++)
        {
            uint32_t index = y * 2 + x;

            mr_set_font(&mr, test_fonts[index]);
            mr_set_stroke_color(&mr, mr_get_color(0xDF1B1B));
            mr_set_fill_color(&mr,
                              mr_get_color((x != y) ? 0xf7f7f7 : 0xe8ecf2));

            rectangle = (mr_rectangle_t){CONTENT_X + x * CELL_WIDTH,
                                         CONTENT_Y + y * CELL_HEIGHT,
                                         CELL_WIDTH,
                                         CELL_HEIGHT};
            offset = (mr_point_t){
                (CELL_WIDTH - mr_get_utf8_text_width(&mr,
                                                     (uint8_t *)test_strings[index])) /
                    2,
                (CELL_HEIGHT - mr_get_line_height(&mr)) / 2};

            mr_draw_utf8_text(&mr,
                              (uint8_t *)test_strings[index],
                              &rectangle,
                              &offset);
        }
    }

    // Draw status bar
    mr_set_fill_color(&mr, mr_get_color(0xffffff));

    const char *statusbar_items[] = {
        "Hello world!",
        "12:34",
        "\xee\x86\xa7",
        "\xee\x86\xa4"};
    const uint32_t statusbar_x[] = {
        0,
        STATUSBAR_WIDTH - 124,
        STATUSBAR_WIDTH - 68,
        STATUSBAR_WIDTH - 40,
        STATUSBAR_WIDTH};

    rectangle = (mr_rectangle_t){STATUSBAR_X,
                                 STATUSBAR_Y,
                                 STATUSBAR_WIDTH / 2,
                                 STATUSBAR_HEIGHT};

    for (int i = 0; i < STATUSBAR_ITEMS; i++)
    {
        rectangle.x = statusbar_x[i];
        rectangle.width = statusbar_x[i + 1] - statusbar_x[i];

        if (i < 2)
            mr_set_font(&mr, font_robotoM12_4);
        else
            mr_set_font(&mr, font_material_symbolsR12_4);

        if (i < 1)
            mr_set_stroke_color(&mr, mr_get_color(0x000000));
        else
            mr_set_stroke_color(&mr, mr_get_color(0x707070));

        offset = (mr_point_t){
            FONT_ROBOTOM12_4_CAP_HEIGHT,
            (STATUSBAR_HEIGHT - mr_get_line_height(&mr)) / 2};

        mr_draw_utf8_text(&mr,
                          (uint8_t *)statusbar_items[i],
                          &rectangle,
                          &offset);
    }

    set_display(&mr, true);

    // Update screen
    while (true)
        update_display(&mr);
}
