![Project logo](img/mcu-renderer-title.png)

# mcu-renderer

mcu-renderer is a C-language graphics library for MCUs, focused on rendering non-flickering, anti-aliased text with low resource use on both monochrome and color LCD displays.

## Features

* Focused on MCUs with little memory.
* Common API for monochrome and color displays.
* Very fast anti-aliased font rendering.
* High font compression ratio.
* Adjustable font bit-depth.
* C-string, UTF-8 and UTF-16 support.
* On color displays: requires only a small RGB565 framebuffer covering two or more characters.
* On monochrome displays: requires a full framebuffer.
* SDL graphics for testing on a PC.

## Supported devices

* [ILI9341](docs/supported-devices.md#ili9341)
* [ST7789](docs/supported-devices.md#st7789)
* [ST7565](docs/supported-devices.md#st7565)

## Setup

To start using mcu-renderer, include the [appropriate header file](docs/supported-devices.md).

In order to initialize the library, call the corresponding `mr_init_xxx()` function. The display is initially turned off, allowing you to draw before the first screen update. To turn the display on, call `mr_xxx_set_display()`.

Next, set up the fonts. You may use the fonts available in the [fonts](fonts folder), or prepare your own, as the [Preparing fonts](#preparing-fonts) section explains.

After that, set up a screen layout consisting of non-overlapping rectangles. This will avoid any flickering when updating the screen.

To draw an filled rectangle, call `mr_set_fill_color()` to set the fill color and `mr_draw_rectangle()` for the actual drawing. You can convert web colors (e.g. #ff2020) to mcu-renderer colors with the `mr_get_color` macro.

To draw an image in a user-provided RGB565 framebuffer, call `mr_draw_image()`.

To draw a text rectangle, call `mr_set_font()` to set the font, `mr_set_fill_color()` to set the background color, `mr_set_text_color()` to set the text color, and `mr_draw_text()`, `mr_draw_text_utf8()` and `mr_draw_text_utf16()` to render C-strings, UTF-8 strings and UTF-16 strings, respectively. The `offset` parameter specifies the upper left corner of the text within the drawing rectangle: to center text horizontally, use the `mr_get_text_width()`, `mr_get_text_utf8_width()` and `mr_get_text_utf16_width()` functions; to center text vertically, get the current font's metrics with `mr_get_cap_height()` (height of uppercase A), `mr_get_ascent()` (top of line to [baseline](https://en.wikipedia.org/wiki/Baseline_(typography))), `mr_get_descent()` (baseline to bottom of line) and `mr_get_line_height()` (height of line). Make sure you set the font before retrieving any metrics.

Rectangle bounds may not exceed the display size. The `offset` parameter of the text drawing functions is not limited.

Monochrome displays and SDL require a screen refresh. For refreshing the screen, call `mr_refresh_screen()` after you finished drawing.

## Examples

See the [examples](examples) folder.

## Font compression ratio comparison

(ASCII characters 0x20-0x7e)  
(sizes in bytes)

| Font                          | Roboto 12 px | Roboto 24 px | Roboto 48 px | Roboto 72 px |
|-------------------------------|--------------|--------------|--------------|--------------|
| Adafruit_GFX/TFT_eSPI (1 bpp) | 3832         | 14589        | 56677        | 129389       |
| u8g2 (1 bpp)                  | 1508         | 3039         | 6632         | N/A          |
| mcu-renderer 1 bpp            | 1473         | 2891         | 5902         | 9511         |
| mcu-renderer 2 bpp            | 2052         | 4162         | 8743         | 13969        |
| mcu-renderer 3 bpp            | 2695         | 5677         | 12128        | 18701        |
| mcu-renderer 4 bpp            | 3328         | 7329         | 15533        | 23676        |
| MCUFont (4 bpp)               | 3125         | 6296         | 12969        | 20045        |
| TFT_eSPI (vlw)                | 7642         | 19387        | 65833        | 143333       |

## Preparing fonts

In order to convert your fonts to the mcu-renderer format, use the `fontconv` tool, which lets you convert both `.bdf` and `.pcf` bitmap fonts as well as `.ttf`, `.ttc`, `.otf`, `.otc` and `.woff` vector fonts.

In digital typography, 72 points is usually defined as 96 pixels. For rasterizing a vector font to a given pixel size, convert the pixel size to a point size by multiplying by the factor 4/3. Not all fonts follow this rule, so you might need to tweak this factor.

`fontconv` calculates the cap height (the height of the uppercase letter A) as the difference between the ascent height (top of line to baseline) and the descent height (baseline to bottom of line). On some fonts this calculation is inaccurate, so you can override the cap height to achieve the correct result. You can also override the ascender height and the descender height.

## Additional resources

* [Roboto font](https://fonts.google.com/specimen/Roboto): great font for embedded systems
* [Material symbols](https://fonts.google.com/icons): great symbol font
* [Font forge](https://fontforge.org/): vector font editor
* [Bits'N'Picas](https://github.com/kreativekorp/bitsnpicas): bitmap font editor
* [FontDrop!](https://fontdrop.info/): online glyph viewer
* [Oh My Glyph](https://www.ohmyglyph.com/): online glyph copy+paste
