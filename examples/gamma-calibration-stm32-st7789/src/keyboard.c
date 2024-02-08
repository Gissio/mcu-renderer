/*
 * MCU renderer
 * STM32 keyboard
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include <libopencm3/stm32/gpio.h>

#include "keyboard.h"

void initKeyboard(void)
{
    gpio_set(GPIOA,
             GPIO8 | GPIO9 | GPIO10 | GPIO11);
    gpio_set_mode(GPIOA,
                  GPIO_MODE_INPUT,
                  GPIO_CNF_INPUT_PULL_UPDOWN,
                  GPIO8 | GPIO9 | GPIO10 | GPIO11);
}

bool getKeyDown(key_t index)
{
    switch (index)
    {
    case KEY_LEFT:
        return !gpio_get(GPIOA, GPIO8);

    case KEY_RIGHT:
        return !gpio_get(GPIOA, GPIO9);

    case KEY_UP:
        return !gpio_get(GPIOA, GPIO10);

    case KEY_DOWN:
        return !gpio_get(GPIOA, GPIO11);
    }
}
