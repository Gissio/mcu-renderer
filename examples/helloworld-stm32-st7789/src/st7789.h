/*
 * MCU renderer
 * ST7789 on STM32
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#if !defined(ST7789_H)
#define ST7789_H

#include "stdint.h"

#include "mcu-renderer-st7789.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240
#define DISPLAY_ROTATION MR_DISPLAY_ROTATION_90

void initST7789(mr_t *mr);

#endif
