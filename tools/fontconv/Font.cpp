/*
 * MCU renderer fontconv
 * Font data structure
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <climits>

#include "Font.h"

MinMax::MinMax()
{
    min = INT_MAX;
    max = INT_MIN;
}

void MinMax::update(int32_t value)
{
    if (value < min)
        min = value;
    if (value > max)
        max = value;
}

Font::Font()
{
    capHeight = 0;
    ascent = INT_MIN;
    descent = INT_MIN;

    boundingBoxLeft = INT_MAX;
    boundingBoxBottom = INT_MAX;
    boundingBoxWidth = 0;
    boundingBoxHeight = 0;
}

void Font::add(Charcode charcode, Glyph &glyph)
{
    int32_t fontRight;
    int32_t fontTop;
    int32_t glyphRight;
    int32_t glyphTop;

    fontRight = boundingBoxWidth
                    ? (boundingBoxLeft + boundingBoxWidth)
                    : INT_MIN;
    fontTop = boundingBoxHeight
                  ? (boundingBoxBottom + boundingBoxHeight)
                  : INT_MIN;

    glyphRight = glyph.left + glyph.width;
    glyphTop = glyph.bottom + glyph.height;

    if (glyph.left < boundingBoxLeft)
        boundingBoxLeft = glyph.left;
    if (glyph.bottom < boundingBoxBottom)
        boundingBoxBottom = glyph.bottom;
    if (glyphRight > fontRight)
        fontRight = glyphRight;
    if (glyphTop > fontTop)
        fontTop = glyphTop;

    boundingBoxWidth = fontRight - boundingBoxLeft;
    boundingBoxHeight = fontTop - boundingBoxBottom;

    boundingBoxLeftMinMax.update(glyph.left);
    boundingBoxBottomMinMax.update(glyph.bottom);
    boundingBoxWidthMinMax.update(glyph.width);
    boundingBoxHeightMinMax.update(glyph.height);
    advanceMinMax.update(glyph.advance);

    glyphs[charcode] = glyph;
}
