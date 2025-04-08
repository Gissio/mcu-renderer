/*
 * MCU renderer fontconv
 * Main module
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <fstream>
#include <iostream>
#include <set>

#include "encode.h"
#include "export.h"
#include "import_bdf.h"
#include "import_freetype.h"
#include "utils.h"

#define DEFAULT_POINT_SIZE 12
#define DEFAULT_BITS_PER_PIXEL 4

using namespace std;

void printHelp(void)
{
    cout << "fontconv [options] input-file output-file\n"
         << "-h          Display this help.\n"
         << "-p <size>   Set point size for rasterizing fonts (default: " << DEFAULT_POINT_SIZE << ").\n"
         << "-b <size>   Set bits per pixel (default: " << DEFAULT_BITS_PER_PIXEL << ").\n"
         << "-s <subset> Specify a subset of Unicode characters to convert.\n"
         << "-u <filename> Specify a text file with utf8 characters to convert.\n"
         << "-c <size>   Override the font cap height.\n"
         << "-a <size>   Override the font ascent (baseline to top of line).\n"
         << "-d <size>   Override the font descent (bottom of line to baseline).\n"
         << "-n <name>   Override the C-language font variable name.\n"
         << "\n"
         << "example:\n"
         << "  -s 32-255            select Unicode characters 32 to 255.\n"
         << "  -s 0x2e,0x30-0x39    select space and digit characters.\n";

    exit(1);
}

int main(int argc, char **argv)
{
    // Parse command line

    vector<string> args(argv + 1, argv + argc);

    string charcodes;
    uint32_t pointSize = DEFAULT_POINT_SIZE;
    uint32_t pixelBitNum = DEFAULT_BITS_PER_PIXEL;
    int32_t overrideCapHeight = INT_MIN;
    int32_t overrideAscent = INT_MIN;
    int32_t overrideDescent = INT_MIN;
    string variableName;
    string inputFilename;
    string outputFilename;
    string textFilename;

    for (uint32_t i = 0; i < args.size(); i++)
    {
        if (args[i].compare("-h") == 0)
            printHelp();
        else if (args[i].compare("-p") == 0)
        {
            i++;

            if (i < args.size())
            {
                int32_t value = stoi(args[i]);

                if ((value >= 1) && (value <= 512))
                    pointSize = value;
            }
        }
        else if (args[i].compare("-b") == 0)
        {
            i++;

            if (i < args.size())
            {
                int32_t value = stoi(args[i]);

                if ((value >= 1) &&
                    (value <= 8))
                    pixelBitNum = value;
            }
        }
        else if (args[i].compare("-s") == 0)
        {
            i++;

            if (i < args.size())
                charcodes = args[i];
        }
        else if (args[i].compare("-c") == 0)
        {
            i++;

            if (i < args.size())
                overrideCapHeight = stoi(args[i]);
        }
        else if (args[i].compare("-a") == 0)
        {
            i++;

            if (i < args.size())
                overrideAscent = stoi(args[i]);
        }
        else if (args[i].compare("-d") == 0)
        {
            i++;

            if (i < args.size())
                overrideDescent = stoi(args[i]);
        }
        else if (args[i].compare("-n") == 0)
        {
            i++;

            if (i < args.size())
                variableName = args[i];
        }
        else if (args[i].compare("-u") == 0)
        {
            i++;

            if (i < args.size())
                textFilename = args[i];
        }
        else if (inputFilename == "")
            inputFilename = args[i];
        else if (outputFilename == "")
            outputFilename = args[i];
        else
            printHelp();
    }

    if ((inputFilename == "") || (outputFilename == ""))
        printHelp();

    // Process font

    try
    {
        Font font;

        set<Charcode> charcodeSet = parseCharcodes(charcodes);

        if (!textFilename.empty())
            charcodeSet.merge(parseUtf8(textFilename));

        if (to_lower(inputFilename).ends_with(".bdf"))
        {
            font = loadBDFFont(inputFilename,
                               charcodeSet);

            pixelBitNum = 1;
        }
        else
            font = loadFreeTypeFont(inputFilename,
                                    charcodeSet,
                                    pointSize);

        if (overrideCapHeight != INT_MIN)
            font.capHeight = overrideCapHeight;
        if (overrideAscent != INT_MIN)
            font.ascent = overrideAscent;
        if (overrideDescent != INT_MIN)
            font.descent = overrideDescent;

        BitWriter encodedFont;
        encodeFont(font,
                   pixelBitNum,
                   encodedFont);

        exportFont(font,
                   variableName,
                   charcodes,
                   encodedFont.data,
                   outputFilename);
    }
    catch (runtime_error &e)
    {
        cerr << "error: " << e.what() << endl;
    }

    return 0;
}
