/*
 * MCU renderer
 * STM32 system code
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#if !defined(STM32_H)
#define STM32_H

#include <stdint.h>

#define SYSTICK_FREQUENCY 1000

void initSystem(void);

void sleep(uint32_t value);

#endif
