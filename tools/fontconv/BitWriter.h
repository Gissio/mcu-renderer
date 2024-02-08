/*
 * MCU renderer fontconv
 * Bit writer
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#if !defined(BITWRITER_H)
#define BITWRITER_H

#include <cstdint>
#include <vector>

class BitWriter
{
public:
    BitWriter();

    void clear();

    void writeBit(bool value);
    void writeFixedEncodedValue(uint32_t value, uint32_t bitNum);
    void writeUnaryEncodedValue(uint32_t value);
    void writeRiceEncodedValue(uint32_t value, uint32_t fixedBitNum);

    void writeByte(uint8_t value);
    void writeByte(uint32_t address, uint8_t value);

    void writeShort(int16_t value);
    void writeShort(uint32_t address, int16_t value);

    void writeVariableLengthWord(uint32_t value);

    void write(BitWriter &value);

    bool readBit(uint32_t bitIndex);

    uint32_t getBitNum();
    uint32_t getCurrentAddress();

    bool operator!=(BitWriter &value);

    std::vector<uint8_t> data;
    uint8_t bitIndex;
};

#endif
