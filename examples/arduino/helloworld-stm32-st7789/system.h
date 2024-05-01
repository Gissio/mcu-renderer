/*
 * MCU renderer
 * Arduino system code
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#if !defined(SYSTEM_H)
#define SYSTEM_H

#include <stdint.h>

void init_system(void);

void sleep(uint32_t value);

#endif
