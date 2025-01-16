/*
 * MCU renderer
 * ST7789 on STM32
 *
 * (C) 2023-2025 Gissio
 *
 * License: MIT
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "display.h"
#include "system.h"

// Connections

#define DISPLAY_RESX_PORT GPIOB
#define DISPLAY_RESX_PIN GPIO0
#define DISPLAY_CSX_PORT GPIOB
#define DISPLAY_CSX_PIN GPIO1
#define DISPLAY_DCX_PORT GPIOB
#define DISPLAY_DCX_PIN GPIO10
#define DISPLAY_SCL_PORT GPIOA
#define DISPLAY_SCL_PIN GPIO5
#define DISPLAY_SDA_PORT GPIOA
#define DISPLAY_SDA_PIN GPIO7

// Display

static uint8_t display_text_buffer[80 * 80];

static const uint8_t display_init_sequence[] = {
    MR_SEND_COMMAND(MR_ST7789_INVON), // Inverse for IPS displays
    MR_END(),
};

void on_display_sleep(uint32_t value);
void on_display_set_reset(bool value);
void on_display_set_command(bool value);
void on_display_send(uint16_t value);
void on_display_send16(uint16_t value);

void on_display_sleep(uint32_t value)
{
    sleep(value);
}

void on_display_set_reset(bool value)
{
    if (value)
        gpio_clear(DISPLAY_RESX_PORT,
                   DISPLAY_RESX_PIN);
    else
        gpio_set(DISPLAY_RESX_PORT,
                 DISPLAY_RESX_PIN);
}

void on_display_set_chipselect(bool value)
{
    if (value)
        gpio_clear(DISPLAY_CSX_PORT,
                   DISPLAY_CSX_PIN);
    else
        gpio_set(DISPLAY_CSX_PORT,
                 DISPLAY_CSX_PIN);
}

void on_display_set_command(bool value)
{
    if (value)
        gpio_clear(DISPLAY_DCX_PORT,
                   DISPLAY_DCX_PIN);
    else
        gpio_set(DISPLAY_DCX_PORT,
                 DISPLAY_DCX_PIN);
}

void on_display_send(uint16_t value)
{
    spi_send(SPI1, value);
}

void on_display_send16(uint16_t value)
{
    spi_send(SPI1, (value >> 8) & 0xff);
    spi_send(SPI1, (value >> 0) & 0xff);
}

void init_display(mr_t *mr)
{
    // Setup GPIO
    gpio_set(DISPLAY_RESX_PORT,
             DISPLAY_RESX_PIN);
    gpio_set(DISPLAY_CSX_PORT,
             DISPLAY_CSX_PIN);

    gpio_set_mode(DISPLAY_RESX_PORT,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL,
                  DISPLAY_RESX_PIN);
    gpio_set_mode(DISPLAY_CSX_PORT,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL,
                  DISPLAY_CSX_PIN);
    gpio_set_mode(DISPLAY_DCX_PORT,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL,
                  DISPLAY_DCX_PIN);
    gpio_set_mode(DISPLAY_SCL_PORT,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  DISPLAY_SCL_PIN);
    gpio_set_mode(DISPLAY_SDA_PORT,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  DISPLAY_SDA_PIN);

    // Setup SPI
    rcc_periph_clock_enable(RCC_SPI1);
    spi_init_master(SPI1,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_enable(SPI1);

    // Setup display
    mr_st7789_init(mr,
                   DISPLAY_HEIGHT,
                   DISPLAY_WIDTH,
                   MR_DISPLAY_ROTATION_270,
                   display_text_buffer,
                   sizeof(display_text_buffer),
                   on_display_sleep,
                   on_display_set_reset,
                   on_display_set_chipselect,
                   on_display_set_command,
                   on_display_send,
                   on_display_send16);

    mr_send_sequence(mr, display_init_sequence);
}

void set_display(mr_t *mr, bool value)
{
    mr_st7789_set_display(mr, value);
    mr_st7789_set_sleep(mr, !value);
}

void update_display(mr_t *mr)
{
}
