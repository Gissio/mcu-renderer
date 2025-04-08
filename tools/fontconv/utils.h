/*
 * MCU renderer fontconv
 * Utilities
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(UTILS_H)
#define UTILS_H

#include <set>
#include <string>

#include "Font.h"

std::string to_lower(std::string str);
std::string to_upper(std::string str);
std::vector<std::string> split(std::string &str, char c);
std::set<Charcode> parseCharcodes(std::string &charcodes);
std::set<Charcode> parseUtf8(std::string &filename);

#endif
