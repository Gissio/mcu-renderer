/*
 * MCU renderer
 * STM32 system code
 *
 * (C) 2023-2025 Gissio
 *
 * License: MIT
 */

#if !defined(SYSTEM_H)
#define SYSTEM_H

#include <stdint.h>

#define SYSTICK_FREQUENCY 1000

void init_system(void);

void sleep(uint32_t value);

#endif
