/*
 * MCU renderer
 * Arduino system code
 *
 * (C) 2023-2024 Gissio
 *
 * License: MIT
 */

#include <Arduino.h>
#include <SPI.h>

#include "display.h"
#include "system.h"

// Connections

#define DISPLAY_RESX PB0
#define DISPLAY_CSX PB1
#define DISPLAY_DCX PB10
#define DISPLAY_SCL PA5
#define DISPLAY_SDA PA7

// Display

static uint8_t display_text_buffer[80 * 80];

static const uint8_t display_init_sequence[] = {
    MR_SEND_COMMAND(MR_ST7789_INVON), // Inverse for IPS displays
    MR_END(),
};

void on_display_sleep(uint32_t value);
void on_display_set_reset(bool value);
void on_Display_set_command(bool value);
void on_display_send(uint16_t value);
void on_display_send16(uint16_t value);

void on_display_sleep(uint32_t value)
{
  delay(value);
}

void on_display_set_reset(bool value)
{
  digitalWrite(DISPLAY_RESX, !value);
}

void on_display_set_reset(bool value)
{
  digitalWrite(DISPLAY_CSX, !value);
}

void on_display_set_command(bool value)
{
  digitalWrite(DISPLAY_DCX, !value);
}

void on_display_send(uint16_t value)
{
  SPI.transfer(value);
}

void on_display_send16(uint16_t value)
{
  SPI.transfer((value >> 8) & 0xff);
  SPI.transfer((value >> 0) & 0xff);
}

void init_display(mr_t *mr)
{
  // Setup GPIO
  pinMode(DISPLAY_RESX, OUTPUT);
  pinMode(DISPLAY_CSX, OUTPUT);
  pinMode(DISPLAY_DCX, OUTPUT);

  digitalWrite(DISPLAY_RESX, HIGH);
  digitalWrite(DISPLAY_CSX, HIGH);

  // Setup SPI
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE3));

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
