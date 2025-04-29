# Supported devices

This document outlines the supported display devices for the `mcu-renderer` library, including initialization instructions and configuration details for each.

## Overview

The `mcu-renderer` library supports multiple display types, including SDL for PC-based testing and hardware displays like ST7789, ILI9341, and ST7565. Each display requires specific initialization functions and configurations, detailed below. Ensure you include the appropriate header file and define necessary preprocessor symbols for your target display.

## SDL (PC testing)

The SDL backend is ideal for testing graphics output on a PC, simulating various display types.

### Setup
- **Include**: `mcu-renderer-sdl.h`
- **Preprocessor Symbol**: Define `MCURENDERER_SDL` to compile `mcu-renderer-sdl.c`.

### Initialization
Call `mr_sdl_init()` to set up the SDL display:

```c
void mr_sdl_init(mr_t *mr,
                 uint32_t width,
                 uint32_t height,
                 enum mr_sdl_display_type_t display_type,
                 uint32_t upscale,
                 const char *title);
```

#### Parameters
- `mr`: Pointer to the `mcu-renderer` context.
- `width`, `height`: Physical display dimensions in pixels.
- `display_type`: Specifies the display simulation:
  - `MR_SDL_DISPLAY_TYPE_COLOR`: Simulates a color display.
  - `MR_SDL_DISPLAY_TYPE_MONOCHROME`: Simulates a monochrome display.
- `upscale`: Scaling factor for the SDL window (e.g., `2` doubles the window size).
- `title`: Window title for the SDL display.

#### Example
```c
mr_sdl_init(&mr, 320, 240, MR_SDL_DISPLAY_TYPE_COLOR, 2, "mcu-renderer Test");
```

## ST7789 (Color LCD)

The ST7789 is a color LCD display commonly used in embedded systems.

### Setup
- **Include**: `mcu-renderer-st7789.h`

### Initialization
Call `mr_st7789_init()` to initialize the display:

```c
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
```

#### Parameters
- `mr`: Pointer to the `mcu-renderer` context.
- `width`, `height`: Physical display dimensions in pixels.
- `rotation`: Screen orientation (`mr_display_rotation_t` enum, e.g., `MR_ROTATION_0`, `MR_ROTATION_90`).
- `textbuffer`: User-provided framebuffer for text rendering.
  - Minimum size: `2 * font_boundingbox_width * font_boundingbox_height` bytes.
  - Insufficient size may cause text rendering issues.
- `textbuffer_size`: Size of the `textbuffer` in bytes.
- Callbacks:
  - `sleep_callback`: Delays the processor (in milliseconds).
  - `set_reset_callback`: Sets the reset line (RESX or RES).
  - `set_chipselect_callback`: Sets the chip select line (CS).
  - `set_command_callback`: Sets the command/data line (DCX, DC, or A0).
  - `send_callback`: Sends 8-bit data to the display.
  - `send16_callback`: Sends 16-bit data to the display.

#### Post-initialization
The initialization is minimal. Send a custom initialization sequence using `mr_send_sequence()` after calling `mr_st7789_init()`.

#### Example
```c
uint8_t textbuffer[256];
mr_st7789_init(&mr, 240, 320, MR_ROTATION_0, textbuffer, sizeof(textbuffer),
               sleep_ms, set_reset, set_cs, set_dc, send_8bit, send_16bit);
mr_send_sequence(&mr, st7789_init_sequence, sizeof(st7789_init_sequence));
```

## ILI9341 (Color LCD)

The ILI9341 is similar to the ST7789, with minor differences in setup.

### Setup
- **Include**: `mcu-renderer-ili9341.h`

### Initialization
Call `mr_ili9341_init()` with the same parameters as `mr_st7789_init()`.

#### Notes
- Configuration and callbacks are identical to the ST7789.
- Post-initialization requires a custom sequence via `mr_send_sequence()`.

#### Example
```c
uint8_t textbuffer[256];
mr_ili9341_init(&mr, 240, 320, MR_ROTATION_0, textbuffer, sizeof(textbuffer),
                sleep_ms, set_reset, set_cs, set_dc, send_8bit, send_16bit);
mr_send_sequence(&mr, ili9341_init_sequence, sizeof(ili9341_init_sequence));
```

## ST7565 (Monochrome LCD)

The ST7565 is a monochrome LCD display for low-power applications.

### Setup
- **Include**: `mcu-renderer-st7565.h`

### Initialization
Call `mr_st7565_init()` to initialize the display:

```c
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
```

#### Parameters
- `mr`: Pointer to the `mcu-renderer` context.
- `width`, `height`: Physical display dimensions in pixels.
- `rotation`: Screen orientation (`mr_display_rotation_t` enum).
- `framebuffer`: User-provided framebuffer.
  - Size: `(width * (height + 7) / 8)` bytes.
- Callbacks:
  - `sleep_callback`: Delays the processor (in milliseconds).
  - `set_reset_callback`: Sets the reset line (RESX or RES).
  - `set_chipselect_callback`: Sets the chip select line (CSX or CS).
  - `set_command_callback`: Sets the command/data line (DCX, DC, or A0).
  - `send_callback`: Sends 8-bit data to the display.

#### Post-initialization
Send a custom initialization sequence using `mr_send_sequence()` after calling `mr_st7565_init()`.

#### Example
```c
uint8_t framebuffer[1024]; // For 128x64 display
mr_st7565_init(&mr, 128, 64, MR_ROTATION_0, framebuffer,
               sleep_ms, set_reset, set_cs, set_dc, send_8bit);
mr_send_sequence(&mr, st7565_init_sequence);
```

## Best Practices
- **Framebuffer sizing**: Ensure framebuffers meet minimum size requirements to avoid rendering issues.
- **Custom initialization**: Always follow initialization calls with `mr_send_sequence()` to configure the display properly.
- **Callback implementation**: Implement callbacks carefully, ensuring they interact correctly with your hardware's GPIO and communication protocols.
- **Testing with SDL**: Use the SDL backend to validate rendering logic before deploying to hardware.

## Troubleshooting
- **Text not rendering (ST7789/ILI9341)**: Verify that `textbuffer` is large enough.
- **Display not responding**: Check callback implementations and ensure the initialization sequence is correct.
- **SDL window issues**: Confirm the `MCURENDERER_SDL` symbol is defined and SDL libraries are linked.
