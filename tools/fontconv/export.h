/*
 * MCU renderer fontconv
 * Font export
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(EXPORT_C_H)
#define EXPORT_C_H

#include "Font.h"

void exportFont(Font &font,
                 std::string variableName,
                 std::string charcodes,
                 std::vector<uint8_t> &fontData,
                 std::string filename);

#endif
