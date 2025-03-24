/*
 * MCU renderer fontconv
 * Bit writer
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include "BitWriter.h"

BitWriter::BitWriter()
{
    bitIndex = 0;
}

void BitWriter::clear()
{
    data.clear();
    bitIndex = 0;
}

void BitWriter::writeBit(bool value)
{
    if (bitIndex == 0)
        data.push_back(0);

    data.back() |= (value << bitIndex);
    bitIndex = (bitIndex + 1) & 0x7;
}

void BitWriter::writeFixedEncodedValue(uint32_t value, uint32_t bitNum)
{
    while (bitNum--)
    {
        writeBit(value & 0x1);
        value >>= 1;
    }
}

void BitWriter::writeUnaryEncodedValue(uint32_t value)
{
    while (value--)
        writeBit(1);
    writeBit(0);
}

void BitWriter::writeRiceEncodedValue(uint32_t value, uint32_t fixedBitNum)
{
    uint32_t remainder = value & ((1 << fixedBitNum) - 1);
    uint32_t quotient = value >> fixedBitNum;

    writeFixedEncodedValue(remainder, fixedBitNum);
    writeUnaryEncodedValue(quotient);
}

void BitWriter::writeByte(uint8_t value)
{
    data.push_back(value);
    bitIndex = 0;
}

void BitWriter::writeByte(uint32_t address, uint8_t value)
{
    data[address] = value;
}

void BitWriter::writeShort(int16_t value)
{
    data.push_back((value >> 8) & 0xff);
    data.push_back((value >> 0) & 0xff);
    bitIndex = 0;
}

void BitWriter::writeShort(uint32_t address, int16_t value)
{
    data[address] = (value >> 8) & 0xff;
    data[address + 1] = (value >> 0) & 0xff;
}

void BitWriter::writeVariableLengthWord(uint32_t value)
{
    if (!value)
        data.push_back(0);
    else
    {
        for (int32_t shift = 28; shift >= 0; shift -= 7)
        {
            uint32_t shiftedValue = value >> shift;

            if (!shiftedValue)
                continue;

            shiftedValue &= 0x7f;

            if (shift)
                shiftedValue |= 0x80;

            data.push_back(shiftedValue);
        }
    }

    bitIndex = 0;
}

void BitWriter::write(BitWriter &value)
{
    if (bitIndex == 0)
    {
        data.insert(data.end(),
                    value.data.begin(),
                    value.data.end());

        bitIndex = value.bitIndex;
    }
    else
    {
        for (size_t i = 0; i < value.getBitNum(); i++)
            writeBit(value.readBit(i));
    }
}

bool BitWriter::readBit(uint32_t bitIndex)
{
    return data[bitIndex / 8] &
           (1 << (bitIndex & 0x7));
}

uint32_t BitWriter::getBitNum()
{
    uint32_t n = (uint32_t)data.size();

    return (bitIndex == 0)
               ? (8 * n)
               : (8 * (n - 1) + bitIndex);
}

uint32_t BitWriter::getCurrentAddress()
{
    return (uint32_t)data.size();
}

bool BitWriter::operator!=(BitWriter &value)
{
    return (bitIndex != value.bitIndex) ||
           (data != value.data);
}
