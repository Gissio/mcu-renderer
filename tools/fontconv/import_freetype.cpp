/*
 * MCU renderer fontconv
 * FreeType import
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SFNT_NAMES_H
#include FT_TRUETYPE_IDS_H

#include "import_freetype.h"

using namespace std;

static string convertFromUTF16(const FT_Byte *str, int size)
{
    string s;

    while (size > 0)
    {
        s.push_back((str[0] << 8) +
                    (str[1] << 0));
        str += 2;
        size -= 2;
    }

    return s;
}

static void checkFTError(FT_Error value, string message)
{
    if (value == 0)
        return;

    throw runtime_error(message +
                        " (freefont error code " +
                        to_string(value) +
                        ")");
}

void getFontInfo(FT_Face &face,
                 string filename,
                 uint32_t pointSize,
                 Font &font)
{
    string fontFamily;
    string fontSubFamily;

    uint32_t count = FT_Get_Sfnt_Name_Count(face);
    for (uint32_t i = 0; i < count; i++)
    {
        FT_SfntName fontName;

        FT_Get_Sfnt_Name(face, i, &fontName);

        string str = convertFromUTF16(fontName.string, fontName.string_len);

        if (fontName.name_id == TT_NAME_ID_COPYRIGHT)
            font.copyright = str;
        else if (fontName.name_id == TT_NAME_ID_FONT_FAMILY)
            fontFamily = str;
        else if (fontName.name_id == TT_NAME_ID_FONT_SUBFAMILY)
            fontSubFamily = str;
    }

    if ((fontFamily != "") || (fontSubFamily != ""))
        font.name = fontFamily + " " + fontSubFamily;
    else
    {
        string basename = filename.substr(filename.find_last_of("/\\") + 1);

        size_t p = basename.find_last_of('.');
        if (p > 0)
            font.name = basename.substr(0, p);
    }

    if (pointSize)
        font.name += string(" ") + to_string(pointSize);
}

Font loadFreeTypeFont(string filename,
                      set<Charcode> &charcodeSet,
                      uint32_t pointSize)
{
    Font font;

    // Load font

    FT_Library freetype;
    checkFTError(FT_Init_FreeType(&freetype),
                 "cannot initialize");

    FT_Face face;
    checkFTError(FT_New_Face(freetype,
                             filename.c_str(),
                             0,
                             &face),
                 "cannot open font");

    // Process font

    if (FT_IS_SCALABLE(face))
        checkFTError(FT_Set_Pixel_Sizes(face,
                                        pointSize,
                                        0),
                     "cannot set char size");
    else
        pointSize = 0;

    getFontInfo(face,
                filename,
                pointSize,
                font);

    font.capHeight = (face->size->metrics.ascender +
                      face->size->metrics.descender) >>
                     6;
    font.ascent = face->size->metrics.ascender >> 6;
    font.descent = -face->size->metrics.descender >> 6;

    // Process glyphs

    FT_ULong charcode;
    FT_UInt glyphIndex;

    charcode = FT_Get_First_Char(face,
                                 &glyphIndex);

    while (glyphIndex)
    {
        if (charcodeSet.empty() ||
            charcodeSet.contains(charcode))
        {
            try
            {
                checkFTError(FT_Load_Glyph(face,
                                           glyphIndex,
                                           FT_LOAD_RENDER),
                             "cannot load glyph");

                uint32_t pitch = face->glyph->bitmap.width;

                Glyph glyph;

                glyph.left = face->glyph->bitmap_left;
                glyph.bottom = face->glyph->bitmap_top - face->glyph->bitmap.rows;
                glyph.width = face->glyph->bitmap.width;
                glyph.height = face->glyph->bitmap.rows;
                glyph.advance = face->glyph->advance.x >> 6;

                glyph.bitmap.resize(glyph.width * glyph.height);
                for (uint32_t y = 0; y < face->glyph->bitmap.rows; y++)
                    copy(&face->glyph->bitmap.buffer[y * pitch],
                         &face->glyph->bitmap.buffer[y * pitch + glyph.width],
                         &glyph.bitmap[y * glyph.width]);

                font.add(charcode, glyph);
            }
            catch (runtime_error &e)
            {
                cerr << "warning: skipping glyph " << glyphIndex << ": "
                     << e.what() << endl;

                continue;
            }
        }

        charcode = FT_Get_Next_Char(face,
                                    charcode,
                                    &glyphIndex);
    }

    // Free library

    checkFTError(FT_Done_Face(face),
                 "FT_Done_Face");

    checkFTError(FT_Done_FreeType(freetype),
                 "FT_Done_FreeType");

    return font;
}
