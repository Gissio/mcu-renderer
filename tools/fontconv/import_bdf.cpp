/*
 * MCU renderer fontconv
 * BDF import
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <cstdlib>
#include <fstream>
#include <climits>

#include "import_bdf.h"
#include "utils.h"

using namespace std;

enum BDFState
{
    BDF_STATE_ROOT,
    BDF_STATE_PROPERTIES,
    BDF_STATE_CHAR,
    BDF_STATE_BITMAP
};

static string filter(string &line)
{
    string output;

    for (char c : line)
    {
        if (c >= ' ')
            output += c;
    }

    return output;
}

static string getBDFString(string &value)
{
    return value.substr(1, value.size() - 2);
}

static bool getBDFValue(string line, string key, string &value)
{
    if (line.size() > key.size())
        key += " ";

    bool match = to_upper(line).starts_with(to_upper(key));
    if (match)
        value = line.substr(key.size());

    return match;
}

static uint8_t getHexByte(string str)
{
    return strtoul(str.substr(0, 2).c_str(), NULL, 16);
}

Font loadBDFFont(string filename,
                 set<Charcode> &charcodeSet)
{
    Font font;

    ifstream file(filename);
    if (!file.good())
        throw runtime_error("could not open '" +
                            filename +
                            "'");

    string line;
    uint32_t lineNumber = 1;
    BDFState bdfState = BDF_STATE_ROOT;
    string value;

    Charcode charcode;
    Glyph glyph;
    uint32_t glyphY;

    string familyName;
    string weightName;

    while (getline(file, line))
    {
        line = filter(line);

        switch (bdfState)
        {
        case BDF_STATE_ROOT:
            if (getBDFValue(line, "FONT", value))
                font.name = value;
            else if (getBDFValue(line, "SIZE", value))
            {
                vector<string> parameters = split(value, ' ');
                font.capHeight = stoi(parameters[0]);
            }
            else if (getBDFValue(line, "STARTPROPERTIES", value))
                bdfState = BDF_STATE_PROPERTIES;
            else if (getBDFValue(line, "STARTCHAR", value))
                bdfState = BDF_STATE_CHAR;

            break;

        case BDF_STATE_PROPERTIES:
            if (getBDFValue(line, "FAMILY_NAME", value))
                familyName = getBDFString(value);
            else if (getBDFValue(line, "WEIGHT_NAME", value))
                weightName = getBDFString(value);
            else if (getBDFValue(line, "COPYRIGHT", value))
                font.copyright = getBDFString(value);
            else if (getBDFValue(line, "FONT_ASCENT", value))
                font.ascent = stoi(value);
            else if (getBDFValue(line, "FONT_DESCENT", value))
                font.descent = stoi(value);
            else if (getBDFValue(line, "CAP_HEIGHT", value))
                font.capHeight = stoi(value);
            else if (getBDFValue(line, "ENDPROPERTIES", value))
                bdfState = BDF_STATE_ROOT;

            break;

        case BDF_STATE_CHAR:
            if (getBDFValue(line, "ENCODING", value))
                charcode = stoi(value);
            if (getBDFValue(line, "DWIDTH", value))
            {
                vector<string> parameters = split(value, ' ');

                glyph.advance = stoi(parameters[0]);
            }
            else if (getBDFValue(line, "BBX", value))
            {
                vector<string> parameters = split(value, ' ');

                if (parameters.size() != 4)
                    throw runtime_error("bdf file invalid (line " +
                                        to_string(lineNumber) +
                                        ")");

                glyph.width = stoi(parameters[0]);
                glyph.height = stoi(parameters[1]);
                glyph.left = stoi(parameters[2]);
                glyph.bottom = stoi(parameters[3]);

                glyph.bitmap.resize(glyph.width * glyph.height);
            }
            else if (getBDFValue(line, "BITMAP", value))
            {
                bdfState = BDF_STATE_BITMAP;

                glyphY = 0;
            }

            break;

        case BDF_STATE_BITMAP:
            if (getBDFValue(line, "ENDCHAR", value))
            {
                bdfState = BDF_STATE_ROOT;

                if (charcodeSet.empty() ||
                    charcodeSet.contains(charcode))
                    font.add(charcode, glyph);
            }
            else
            {
                uint32_t glyphX = 0;

                while (line.size() >= 2)
                {
                    uint8_t n = getHexByte(line);
                    uint32_t pixelNum = 0;

                    while ((glyphX < glyph.width) && (pixelNum < 8))
                    {
                        uint8_t alpha = (n & 0x80) ? 0xff : 0;
                        glyph.bitmap[glyphY * glyph.width + glyphX] = alpha;

                        n <<= 1;

                        pixelNum++;
                        glyphX++;
                    }

                    line = line.substr(2);
                }

                glyphY++;
            }

            break;
        }

        lineNumber++;
    }

    if (font.ascent == INT_MIN)
        font.ascent = font.boundingBoxBottom + font.boundingBoxHeight;
    if (font.descent == INT_MIN)
        font.descent = font.boundingBoxBottom;

    if (familyName != "")
        font.name = familyName;
    if (weightName != "")
    {
        font.name += " ";
        font.name += weightName;
    }

    return font;
}
