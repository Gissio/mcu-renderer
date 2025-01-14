# Supported devices

## SDL

To test the graphics output on a PC, include the `mcu-renderer-sdl.h` file. You should also define the `MCURENDERER_SDL` preprocessor symbol in your project, so `mcu-renderer-sdl.c` gets compiled.

To initialize, call `mr_sdl_init()`:

    void mr_sdl_init(mr_t *mr,
                     uint32_t width,
                     uint32_t height,
                     enum mr_sdl_display_type_t display_type,
                     uint32_t upscale,
                     const char *title);

`width` and `height` specify the physical display size.

`display_type` allows simulating a color display (`MR_SDL_DISPLAY_TYPE_COLOR`) or a monochrome one (`MR_SDL_DISPLAY_TYPE_MONOCHROME`).

`upscale` lets you upscale the SDL display window.

`title` is the SDL display window's title.

## ST7789

To start using mcu-renderer with an ST7789 color LCD display, first include the `mcu-renderer-st7789.h` file.

To initialize, call:

    void mr_st7789_init(mr_t *mr,
                        int16_t width,
                        int16_t height,
                        mr_display_rotation_t rotation,
                        uint8_t *textbuffer,
                        uint32_t textbuffer_size,
                        mr_sleep_callback_t sleep_callback,
                        mr_set_reset_callback_t set_reset_callback,
                        mr_set_chipselect_callback_t set_chipselect_callback,
                        mr_set_command_callback_t set_command_callback,
                        mr_send_callback_t send_callback,
                        mr_send_callback_t send16_callback);

`width` and `height` specify the physical display size. `rotation` allows you to rotate the screen.

`textbuffer` is a user-provided framebuffer that should be capable of holding at least two characters, of minimum size `2 * font_boundingbox_width * font_boundingbox_height`. If this framebuffer is too small, text will not render correctly.

`sleep_callback`, `set_reset_callback`, `set_chipselect_callback`, `set_command_callback`, `send_callback` and `send16_callback` are user-provided callbacks. They should, respectively, delay the processor a certain number of milliseconds, set the display's reset line (RESX or RES), set the display's chip select line (CS), set the display's command/data line (DCX, DC or A0) and send 8-bit and 16-bit data to the display.

 mcu-renderer's display initialization is bare-bones. After calling `mr_st7789_init()` you should send a customized initialization sequence with `mr_send_sequence()`.

## ILI9341

Use of the ILI9341 display is similar to the ST7789, except:

* you must include the `mcu-renderer-ili9341.h` file.
* you must call the `mr_ili9341_init()` function.

## ST7565

To start using mcu-renderer with an ST7565 monochrome LCD display, include the `mcu-renderer-st7565.h` file.

To initialize, call:

    void mr_st7565_init(mr_t *mr,
                        int16_t width,
                        int16_t height,
                        mr_display_rotation_t rotation,
                        uint8_t *framebuffer,
                        mr_sleep_callback_t sleep_callback,
                        mr_set_reset_callback_t set_reset_callback,
                        mr_set_chipselect_callback_t set_chipselect_callback,
                        mr_set_command_callback_t set_command_callback,
                        mr_send_callback_t send_callback);

`width` and `height` specify the physical display size. `rotation` allows you to rotate the screen.

`framebuffer` is a user-provided framebuffer of size (width * (height + 7) / 8).

`sleep_callback`, `set_reset_callback`, `set_chipselect_callback`, `set_command_callback` and `send_callback` are user-provided callbacks. They should, respectively, delay the processor a certain number of milliseconds, set the display's reset line (RESX or RES),  set the display's chip select line (CSX or CS), set the display's command/data line (DCX, DC or A0) and send 8-bit data to the display.

mcu-renderer's display initialization is bare-bones. After calling `mr_st7565_init()` you should send a customized initialization sequence with `mr_send_sequence()`.
