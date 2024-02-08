/*
 * MCU renderer fontconv
 * Font data structure
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(FONT_H)
#define FONT_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

typedef int32_t Charcode;

class Glyph
{
public:
    int32_t left;
    int32_t bottom;
    int32_t width;
    int32_t height;

    int32_t advance;

    std::vector<uint8_t> bitmap;
};

class MinMax
{
public:
    MinMax();

    void update(int32_t value);

    int32_t min;
    int32_t max;
};


class Font
{
public:
    Font();

    void add(Charcode charcode, Glyph &glyph);

    std::string name;
    std::string copyright;

    int32_t boundingBoxLeft;
    int32_t boundingBoxBottom;
    int32_t boundingBoxWidth;
    int32_t boundingBoxHeight;

    MinMax boundingBoxLeftMinMax;
    MinMax boundingBoxBottomMinMax;
    MinMax boundingBoxWidthMinMax;
    MinMax boundingBoxHeightMinMax;
    MinMax advanceMinMax;

    int32_t capHeight;
    int32_t ascent;
    int32_t descent;

    std::map<Charcode, Glyph> glyphs;
};

#endif
