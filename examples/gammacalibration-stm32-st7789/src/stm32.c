/*
 * MCU renderer
 * STM32 system code
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>

#include <libopencm3/stm32/rcc.h>

#include "stm32.h"

// SysTick

volatile uint32_t eventsCurrentTick;

void sys_tick_handler(void)
{
    eventsCurrentTick++;
}

void initSystem(void)
{
    // Setup MCU clock
    rcc_clock_setup_in_hse_8mhz_out_72mhz();

    // Setup SysTick
    systick_set_frequency(SYSTICK_FREQUENCY, rcc_ahb_frequency);
    systick_clear();
    nvic_set_priority(NVIC_SYSTICK_IRQ, 0xc0);
    systick_interrupt_enable();
    systick_counter_enable();

    // GPIO
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_GPIOC);
}

void sleep(uint32_t value)
{
    uint32_t startTick = eventsCurrentTick;

    while ((eventsCurrentTick - startTick) < value)
        ;
}
