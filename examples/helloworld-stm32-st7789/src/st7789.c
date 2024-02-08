/*
 * MCU renderer
 * ST7789 on STM32
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/spi.h>

#include "stm32.h"
#include "st7789.h"

#define DISPLAY_SCLK_PORT GPIOA
#define DISPLAY_SCLK_PIN GPIO5
#define DISPLAY_SDA_PORT GPIOA
#define DISPLAY_SDA_PIN GPIO7
#define DISPLAY_RESET_PORT GPIOB
#define DISPLAY_RESET_PIN GPIO0
#define DISPLAY_CS_PORT GPIOB
#define DISPLAY_CS_PIN GPIO1
#define DISPLAY_RS_PORT GPIOB
#define DISPLAY_RS_PIN GPIO10

// Display

static uint8_t textbuffer[80 * 80];

static const uint8_t st7789_init_sequence[] = {
    MR_SEND_COMMAND(MR_ST7789_INVON), // Inverse for IPS displays

    // GC-01 sequence:
    MR_SEND_COMMAND(MR_ST7789_VCOMS),
    MR_SEND_DATA(0x28), // Set: 1.1 V (default 0.9 V)
    MR_SEND_COMMAND(MR_ST7789_PWCTRL1),
    MR_SEND_DATA(0xa4),
    MR_SEND_DATA(0xa1), // Set: AVDD 6.8 V, AVCL -4.8 V, VDDS 2.3 V
    MR_SEND_COMMAND(MR_ST7789_RAMCTRL),
    MR_SEND_DATA(0x00),
    MR_SEND_DATA(0xe0),
    MR_END(),
};

void onSleep(uint32_t value);
void onSetReset(bool value);
void onSetCommand(bool value);
void onSend(uint16_t value);
void onSend16(uint16_t value);

void onSleep(uint32_t value)
{
    sleep(value);
}

void onSetReset(bool value)
{
    if (value)
        gpio_clear(DISPLAY_RESET_PORT,
                   DISPLAY_RESET_PIN);
    else
        gpio_set(DISPLAY_RESET_PORT,
                 DISPLAY_RESET_PIN);
}

void onSetCommand(bool value)
{
    if (value)
    {
        // Trigger CS before command
        gpio_set(DISPLAY_CS_PORT, DISPLAY_CS_PIN);
        gpio_clear(DISPLAY_CS_PORT, DISPLAY_CS_PIN);

        gpio_clear(DISPLAY_RS_PORT,
                   DISPLAY_RS_PIN);
    }
    else
        gpio_set(DISPLAY_RS_PORT,
                 DISPLAY_RS_PIN);
}

void onSend(uint16_t value)
{
    spi_send(SPI1, value);
}

void onSend16(uint16_t value)
{
    spi_send(SPI1, (value >> 8) & 0xff);
    spi_send(SPI1, (value >> 0) & 0xff);
}

void initST7789(mr_t *mr)
{
    // Setup GPIO
    gpio_set(DISPLAY_CS_PORT, DISPLAY_CS_PIN);
    gpio_set_mode(GPIOB,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_PUSHPULL,
                  DISPLAY_RESET_PIN | DISPLAY_CS_PIN |
                      DISPLAY_RS_PIN);

    // Setup SPI
    rcc_periph_clock_enable(RCC_SPI1);
    gpio_set_mode(GPIOA,
                  GPIO_MODE_OUTPUT_50_MHZ,
                  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL,
                  GPIO4 | DISPLAY_SCLK_PIN | DISPLAY_SDA_PIN);
    spi_init_master(SPI1,
                    SPI_CR1_BAUDRATE_FPCLK_DIV_2,
                    SPI_CR1_CPOL_CLK_TO_1_WHEN_IDLE,
                    SPI_CR1_CPHA_CLK_TRANSITION_2,
                    SPI_CR1_DFF_8BIT,
                    SPI_CR1_MSBFIRST);
    spi_enable_software_slave_management(SPI1);
    spi_set_nss_high(SPI1);
    spi_enable(SPI1);

    mr_st7789_init(mr,
                   DISPLAY_WIDTH,
                   DISPLAY_HEIGHT,
                   DISPLAY_ROTATION,
                   textbuffer,
                   sizeof(textbuffer),
                   onSleep,
                   onSetReset,
                   onSetCommand,
                   onSend,
                   onSend16);

    mr_send_sequence(mr, st7789_init_sequence);
}
