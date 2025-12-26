# Releases

## 1.0.16

* Renamed old `mr_get_color` macro to `mr_get_color_rounded`.
* Added new `mr_get_color` for more precise bit RGB888->RGB565 color conversion.
* Improved RGB565->RGB888 color conversion in SDL.

## 1.0.15

* Minor fontconv fix.

## 1.0.14

* Added a [FONTNAME]_SIZE define to fontconv.

## 1.0.13

* Fixes issues in UTF-8 and UTF-16 decoder logic.

## 1.0.10

* Improved UTF-8 and UTF-16 decoder

## 1.0.9

* Added MR_COLOR sample colors.

## 1.0.8

* fontconv now notifies users of requested characters unavailable in the font.
* Renamed fontconv --points parameter to --pixels.
* Improved documentation.

## 1.0.7

* Ported fontconv tool to Python.
* Added textproc tool for calculating Unicode codepoint sets from text/.c files.
* Fixed examples so they compile correctly.

## 1.0.6

* Fixed text buffer error.
* Removed unused mr->buffer_pitch.

## 1.0.5

* Fixed ST7565 buffer overflow.
* Improved handling of chipselect.

## 1.0.4

* Added chipselect callback.

## 1.0.3

* Renamed `mr_set_text_color` to `mr_set_stroke_color`.
* Added `mr_draw_bitmap` for drawing monochrome images.

## 1.0.2

* Minor fixes.

## 1.0.1

* Minor fixes.

## 1.0

* First release.
