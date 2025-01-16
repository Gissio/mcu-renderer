/*
 * MCU renderer example
 * Gamma calibration demo
 *
 * (C) 2023-2025 Gissio
 *
 * License: MIT
 */

#include <stdint.h>
#include <string.h>

#include "display.h"
#include "keyboard.h"
#include "system.h"

#include "mcu-renderer-fonts/font_robotoM24_4.h"

#define GRADIENT_HEIGHT ((DISPLAY_HEIGHT - 40) / 4)

struct gamma_entry
{
    const char *label;
    uint8_t profile_index;
    uint8_t bit_num;
    uint8_t bit_offset;
};

struct gamma_entry gamma_entries[] = {
    {"V0P", 0, 4, 0},
    {"V0N", 0 + 14, 4, 0},
    {"V1P", 1, 6, 0},
    {"V1N", 1 + 14, 6, 0},
    {"V2P", 2, 6, 0},
    {"V2N", 2 + 14, 6, 0},
    {"V4P", 3, 5, 0},
    {"V4N", 3 + 14, 5, 0},
    {"V6P", 4, 5, 0},
    {"V6N", 4 + 14, 5, 0},
    {"V13P", 5, 4, 0},
    {"V13N", 5 + 14, 4, 0},
    {"V20P", 6, 7, 0},
    {"V20N", 6 + 14, 7, 0},
    {"V27P", 7, 3, 0},
    {"V27N", 7 + 14, 3, 0},
    {"V36P", 7, 3, 4},
    {"V36N", 7 + 14, 3, 4},
    {"V43P", 8, 7, 0},
    {"V43N", 8 + 14, 7, 0},
    {"V50P", 9, 4, 0},
    {"V50N", 9 + 14, 4, 0},
    {"V57P", 10, 5, 0},
    {"V57N", 10 + 14, 5, 0},
    {"V59P", 11, 5, 0},
    {"V59N", 11 + 14, 5, 0},
    {"V61P", 12, 6, 0},
    {"V61N", 12 + 14, 6, 0},
    {"V62P", 13, 6, 0},
    {"V62N", 13 + 14, 6, 0},
    {"V63P", 0, 4, 4},
    {"V63N", 0 + 14, 4, 4},
    {"JP0P", 5, 2, 4},
    {"JP0N", 5 + 14, 2, 4},
    {"JP1P", 9, 2, 4},
    {"JP1N", 9 + 14, 2, 4},
};

#define GAMMA_ENTRY_NUM (sizeof(gamma_entries) / sizeof(struct gamma_entry))

uint8_t gamma_profile[14 * 2] = {
    0xd0, 0x00, 0x02, 0x07, 0x0b, 0x1a, 0x31, 0x54, 0x40, 0x29, 0x12, 0x12, 0x12, 0x17,
    0xd0, 0x00, 0x02, 0x07, 0x05, 0x25, 0x2d, 0x44, 0x44, 0x1c, 0x18, 0x16, 0x1c, 0x1d};

void strcat_uint8(char *text, uint8_t value);

void strcat_uint8(char *text, uint8_t value)
{
    uint32_t index = strlen(text);

    uint8_t digit;

    digit = value / 100;
    if (digit)
    {
        text[index++] = '0' + digit;
        value -= 100 * digit;
    }

    digit = value / 10;
    if (digit)
    {
        text[index++] = '0' + digit;
        value -= 10 * digit;
    }

    digit = value;
    text[index++] = '0' + digit;

    text[index] = '\0';
}

// Main

int main(void)
{
    mr_t mr;

    init_system();
    init_display(&mr);
    init_keyboard();

    // Draw color gradients
    mr_rectangle_t rectangle = {
        0, 0,
        1, GRADIENT_HEIGHT};

    for (uint32_t x = 0; x < DISPLAY_WIDTH; x++)
    {
        rectangle.x = x;

        uint32_t value = x * 64 / DISPLAY_WIDTH;
        mr_color_t red = (value >> 1) << 11;
        mr_color_t green = value << 5;
        mr_color_t blue = (value >> 1);

        rectangle.y = 0 * GRADIENT_HEIGHT;
        mr_set_fill_color(&mr, red);
        mr_draw_rectangle(&mr, &rectangle);

        rectangle.y = 1 * GRADIENT_HEIGHT;
        mr_set_fill_color(&mr, green);
        mr_draw_rectangle(&mr, &rectangle);

        rectangle.y = 2 * GRADIENT_HEIGHT;
        mr_set_fill_color(&mr, blue);
        mr_draw_rectangle(&mr, &rectangle);

        rectangle.y = 3 * GRADIENT_HEIGHT;
        mr_set_fill_color(&mr, red | green | blue);
        mr_draw_rectangle(&mr, &rectangle);
    }

    const mr_rectangle_t text_rectangle = {
        0, 4 * GRADIENT_HEIGHT,
        DISPLAY_WIDTH, DISPLAY_HEIGHT - 4 * GRADIENT_HEIGHT};

    int32_t gamma_entry_index = 0;

    while (true)
    {
        // Draw text bar
        mr_send_command(&mr, MR_ST7789_PVGAMCTRL);
        for (uint32_t i = 0; i < 14; i++)
            mr_send_data(&mr, gamma_profile[i]);
        mr_send_command(&mr, MR_ST7789_NVGAMCTRL);
        for (uint32_t i = 14; i < 28; i++)
            mr_send_data(&mr, gamma_profile[i]);

        uint8_t value = gamma_profile[gamma_entries[gamma_entry_index].profile_index];
        uint8_t bit_offset = gamma_entries[gamma_entry_index].bit_offset;
        uint8_t mask = (1 << gamma_entries[gamma_entry_index].bit_num) - 1;
        value >>= bit_offset;
        value &= mask;

        char text[16];
        strcpy(text, gamma_entries[gamma_entry_index].label);
        strcat(text, "=");
        strcat_uint8(text, value);

        mr_set_font(&mr, font_robotoM24_4);
        mr_set_fill_color(&mr, mr_get_color(0x000000));
        mr_set_stroke_color(&mr, mr_get_color(0xffffff));

        const mr_point_t textOffset = {
            (DISPLAY_WIDTH - mr_get_text_width(&mr, text)) / 2, 0};

        mr_draw_text(&mr, text, &text_rectangle, &textOffset);

        set_display(&mr, true);

        // Wait for keyboard event
        while (get_key_down(KEY_LEFT) ||
               get_key_down(KEY_RIGHT) ||
               get_key_down(KEY_UP) ||
               get_key_down(KEY_DOWN))
        {
            sleep(50);
        }

        // Handle keyboard event
        while (true)
        {
            if (get_key_down(KEY_LEFT))
            {
                gamma_entry_index--;
                if (gamma_entry_index < 0)
                    gamma_entry_index = GAMMA_ENTRY_NUM - 1;

                break;
            }

            if (get_key_down(KEY_RIGHT))
            {
                gamma_entry_index++;
                if (gamma_entry_index >= (int32_t)GAMMA_ENTRY_NUM)
                    gamma_entry_index = 0;

                break;
            }

            if (get_key_down(KEY_UP))
            {
                value++;
                value &= mask;
                value <<= bit_offset;
                gamma_profile[gamma_entries[gamma_entry_index].profile_index] &= ~mask;
                gamma_profile[gamma_entries[gamma_entry_index].profile_index] |= value;

                break;
            }

            if (get_key_down(KEY_DOWN))
            {
                value--;
                value &= mask;
                value <<= bit_offset;
                gamma_profile[gamma_entries[gamma_entry_index].profile_index] &= ~mask;
                gamma_profile[gamma_entries[gamma_entry_index].profile_index] |= value;

                break;
            }

            sleep(50);
        }
    }
}
