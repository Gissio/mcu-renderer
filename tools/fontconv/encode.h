/*
 * MCU renderer fontconv
 * Font encoder
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(ENCODER_H)
#define ENCODER_H

#include "Font.h"
#include "BitWriter.h"

void encodeFont(Font &font,
                uint32_t bitsPerPixel,
                BitWriter &bitstream);

#endif
