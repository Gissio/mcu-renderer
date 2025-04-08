/*
 * MCU renderer fontconv
 * Utilities
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include "utils.h"

string to_lower(string str)
{
    for (auto &c : str)
        c = tolower(c);

    return str;
}

string to_upper(string str)
{
    for (auto &c : str)
        c = toupper(c);

    return str;
}

vector<string> split(string &str, char c)
{
    size_t start = 0;
    size_t end;
    vector<string> tokens;

    while ((end = str.find(c, start)) != string::npos)
    {
        tokens.push_back(str.substr(start, end - start));

        start = end + 1;
    }

    tokens.push_back(str.substr(start));

    return tokens;
}

static int convertValue(string &str)
{
    if (str.starts_with("0x"))
        return stoi(str, NULL, 16);
    else
        return stoi(str);
}

set<Charcode> parseCharcodes(string &charcodes)
{
    set<Charcode> charcodeSet;

    vector<string> items = split(charcodes, ',');

    if (charcodes != "")
    {
        for (auto &item : items)
        {
            vector<string> rangeElements = split(item, '-');

            if (rangeElements.size() == 1)
            {
                Charcode value = convertValue(rangeElements[0]);

                if (value >= 0 && value <= UINT16_MAX)
                    charcodeSet.insert(value);
                else
                    throw runtime_error("invalid value");
            }
            else if (rangeElements.size() == 2)
            {
                Charcode from = convertValue(rangeElements[0]);
                Charcode to = convertValue(rangeElements[1]);

                if ((from >= 0) && (to >= 0) && (from <= to))
                {
                    for (Charcode value = from; value <= to; value++)
                        charcodeSet.insert(value);
                }
                else
                    throw runtime_error("invalid range values");
            }
            else
                throw runtime_error("invalid range");
        }
    }

    return charcodeSet;
}

set<Charcode> parseUtf8(string &filename)
{
    const bool utf8_verbose = false;
    set<Charcode> charcodeSet;
    uint8_t byte;
    int glyph;
    int more_bytes;
    int utf8_errs = 0; 

    ifstream file(filename, ios::in | ios::binary);
    if (!file.good())
        throw runtime_error("could not open '" +
                            filename +
                            "'");

    while (file.read(reinterpret_cast<char*>(&byte), 1)) {
        glyph = 0;
        more_bytes = 0;
        
        if (byte <= 0x7F) {
        glyph = byte;
        more_bytes = 0;
        } else if ((byte & 0xE0) == 0xC0) {
            glyph = byte & 0x1F;
            more_bytes = 1;
        } else if ((byte & 0xF0) == 0xE0) {
            glyph = byte & 0x0F;
            more_bytes = 2;
        } else if ((byte & 0xF8) == 0xF0) {
            glyph = byte & 0x07;
            more_bytes = 3;
        } else if ((byte & 0xFC) == 0xF8) {
            glyph = byte & 0x03;
            more_bytes = 4;
        } else if ((byte & 0xFE) == 0xFC) {
            glyph = byte & 0x01;
            more_bytes = 5;
        } else {
            if (utf8_verbose)
                cerr << "? 0x" << hex << byte << endl;
            utf8_errs++;
            continue;
        }
        while (more_bytes > 0) {
            if (!file.read(reinterpret_cast<char*>(&byte), 1))
                break;
            if ((byte & 0xC0) != 0x80) {
                utf8_errs++;
                break;
            }
            glyph = (glyph << 6) | (byte & 0x3F);
            --more_bytes;
        }
        if (more_bytes == 0) {
            if (utf8_verbose)
                cerr << "add 0x" << hex << glyph << endl;
            charcodeSet.insert(glyph); /* add glyph to font */
        }
    }

    if (utf8_errs != 0)
        cerr << utf8_errs << "utf8 errors" << endl;

    file.close();
    return charcodeSet;
}
