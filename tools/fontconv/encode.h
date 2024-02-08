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

#include "font.h"
#include "bitwriter.h"

void encodeFont(Font &font,
                uint32_t bitsPerPixel,
                BitWriter &bitstream);

#endif
