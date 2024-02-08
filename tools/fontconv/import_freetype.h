/*
 * MCU renderer fontconv
 * FreeType import
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(IMPORT_FREETYPE_H)
#define IMPORT_FREETYPE_H

#include <cstdint>
#include <set>
#include <string>

#include "font.h"

Font loadFreeTypeFont(std::string filename,
                      std::set<Charcode> &charcodeSet,
                      uint32_t pointSize);

#endif
