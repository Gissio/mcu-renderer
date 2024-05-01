/*
 * MCU renderer example
 * SDL display
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include "SDL.h"

#include "display.h"

// Display

void init_display(mr_t *mr)
{
    // Setup display
#if defined(DISPLAY_MONOCHROME)
    mr_sdl_init(&mr,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT,
                MR_SDL_DISPLAY_TYPE_MONOCHROME,
                2,
                "mcu-renderer");
#else
    mr_sdl_init(mr,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT,
                MR_SDL_DISPLAY_TYPE_COLOR,
                2,
                "mcu-renderer");
#endif
}

void set_display(mr_t *mr, bool value)
{
    mr_sdl_set_display(mr, value);
    mr_sdl_set_backlight(mr, value ? 255 : 0);
}

void update_display(mr_t *mr)
{
    mr_sdl_refresh_display(mr);

    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            exit(0);
    }
}
