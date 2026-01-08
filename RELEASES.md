# Releases

## 1.0.18

- `fontconv`: added `-e` to write C headers with defines and declarations only.

## 1.0.17

- ST7789: added support for RAMCTRL EPF flags (data translation for 65k/4k frame data) through compiler defines: `MCURENDERER_SDL_R0B0_ZERO`, `MCURENDERER_SDL_R0B0_ONE`, `MCURENDERER_SDL_R0B0_FROM_R5_B5`, `MCURENDERER_SDL_R0B0_FROM_G5`.

## 1.0.16

- API change: old `mr_get_color` renamed to `mr_get_color_rounded` (best for color accuracy).
- Added a new `mr_get_color` implementation that converts RGB888→RGB565 using truncation (best for UIs).
- Improved RGB565→RGB888 conversion in SDL backend.

Migration note: if you used `mr_get_color` previously, update callers or alias the renamed symbol.

## 1.0.15

- Minor `fontconv` bugfix.

## 1.0.14

- `fontconv`: added `[FONTNAME]_SIZE` define to generated headers.

## 1.0.13 / 1.0.10

- Fixed and improved UTF-8 / UTF-16 decoder logic.

## 1.0.9

- Added `MR_COLOR` sample colors.

## 1.0.8

- `fontconv` now warns when requested glyphs are not available in the source font.
- CLI change: renamed `--points` parameter to `--pixels`.

Compatibility note: update any scripts or tooling that used `--points` to use `--pixels`.

## 1.0.7

- Ported `fontconv` tool to Python.
- Added `textproc` helper for computing Unicode codepoint sets from text and C sources.
- Fixed build examples so they compile correctly.

## 1.0.6

- Fixed text buffer bug and removed unused `mr->buffer_pitch`.

## 1.0.5

- Fixed ST7565 buffer overflow and improved chipselect handling.

## 1.0.4

- Added chipselect callback support.

## 1.0.3

- Renamed `mr_set_text_color` to `mr_set_stroke_color`.
- Added `mr_draw_bitmap` to draw monochrome bitmaps.

## 1.0.2 / 1.0.1

- Miscellaneous minor fixes and stability improvements.

## 1.0

- Initial release.
