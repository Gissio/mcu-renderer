/*
 * MCU renderer example
 * SDL display
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#if !defined(DISPLAY_H)
#define DISPLAY_H

#include "mcu-renderer-sdl.h"

#define DISPLAY_WIDTH 320
#define DISPLAY_HEIGHT 240

// Enable for monochrome output:
// #define DISPLAY_MONOCHROME

void init_display(mr_t *mr);
void set_display(mr_t *mr, bool value);
void update_display(mr_t *mr);

#endif
