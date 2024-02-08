/*
 * MCU renderer fontconv
 * Font export
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <iomanip>
#include <fstream>

#include "export.h"

#define BYTES_PER_LINE 16

using namespace std;

static string to_upper(string str)
{
    for (auto &c : str)
        c = toupper(c);

    return str;
}

static string getFontVariableName(string filename)
{
    size_t position;

    position = filename.rfind('/');
    if (position != string::npos)
        filename = filename.substr(position + 1);

    position = filename.rfind('\\');
    if (position != string::npos)
        filename = filename.substr(position + 1);

    position = filename.rfind('.');
    if (position != string::npos)
        filename = filename.substr(0, position);

    string filteredFilename;

    for (char c : filename)
    {
        if (isalpha(c) || isdigit(c))
            filteredFilename += c;
        else
            filteredFilename += '_';
    }

    return filteredFilename;
}

void exportFont(Font &font,
                string variableName,
                string charcodes,
                vector<uint8_t> &fontData,
                string filename)
{
    if (variableName == "")
        variableName = getFontVariableName(filename);

    string constantName = to_upper(variableName);

    if (charcodes == "")
        charcodes = "all";

    ofstream f(filename);

    f << "/**" << endl;
    f << " * Font: " << font.name << endl;
    f << " * Copyright: " << font.copyright << endl;
    f << " * Charcodes: " << charcodes << endl;
    f << " */" << endl;
    f << endl;

    f << "#include <stdint.h>" << endl;
    f << endl;

    f << "#define " << constantName << "_ASCENT "
      << font.ascent << endl;
    f << "#define " << constantName << "_DESCENT "
      << font.descent << endl;
    f << "#define " << constantName << "_CAP_HEIGHT "
      << font.capHeight << endl;
    f << "#define " << constantName << "_LINE_HEIGHT "
      << font.ascent + font.descent << endl;
    f << "#define " << constantName << "_BOUNDINGBOX_LEFT "
      << font.boundingBoxLeft << endl;
    f << "#define " << constantName << "_BOUNDINGBOX_BOTTOM "
      << font.boundingBoxBottom << endl;
    f << "#define " << constantName << "_BOUNDINGBOX_WIDTH "
      << font.boundingBoxWidth << endl;
    f << "#define " << constantName << "_BOUNDINGBOX_HEIGHT "
      << font.boundingBoxHeight << endl;

    f << endl;

    f << "const uint8_t " << variableName << "["
      << fontData.size() << "] =" << endl;
    f << "{\n";

    for (uint32_t i = 0; i < fontData.size(); i++)
    {
        if (i > 0)
        {
            if ((i % BYTES_PER_LINE) == 0)
                f << ",\n";
            else
                f << ", ";
        }

        f << "0x" << hex << setw(2) << setfill('0') << (uint32_t)fontData[i];
    }

    f << "\n";
    f << "};\n";
}
