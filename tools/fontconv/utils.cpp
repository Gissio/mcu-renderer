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
