/*
 * MCU renderer fontconv
 * BDF import
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(IMPORT_BDF_H)
#define IMPORT_BDF_H

#include <cstdint>
#include <set>
#include <string>

#include "font.h"

Font loadBDFFont(std::string filename,
                 std::set<Charcode> &charcodeSet);

#endif
