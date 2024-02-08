/*
 * MCU renderer fontconv
 * Font encoder
 *
 * (C) 2023 Gissio
 *
 * License: MIT
 */

#include <cstring>
#include <iostream>

#include "encode.h"

using namespace std;

struct EncoderSettings
{
    uint32_t boundingBoxLeftBitNum;
    uint32_t boundingBoxBottomBitNum;
    uint32_t boundingBoxWidthBitNum;
    uint32_t boundingBoxHeightBitNum;
    uint32_t advanceBitNum;
    uint32_t pixelBitNum;
    uint32_t runLengthBlackBitNum;
    uint32_t runLengthWhiteBitNum;
};

static uint32_t getBitNum(int32_t value)
{
    if (value < 0)
        value = -value - 1;

    uint32_t bitNum = 0;
    while (value > 0)
    {
        bitNum++;
        value >>= 1;
    }

    return bitNum;
}

static uint32_t getBitNum(MinMax &minMax)
{
    uint32_t bitNumA = getBitNum(minMax.min);
    uint32_t bitNumB = getBitNum(minMax.max);

    return (bitNumA > bitNumB) ? bitNumA : bitNumB;
}

static void encodeGlyph(Glyph &glyph,
                        EncoderSettings &encoderSettings,
                        BitWriter &bitstream)
{
    uint8_t bitshift = (8 - encoderSettings.pixelBitNum);
    uint32_t whiteValue = (1 << encoderSettings.pixelBitNum) - 1;

    vector<BitWriter> symbols;

    // Run-length encoding

    uint32_t symbolIndex = 0;
    while (symbolIndex < glyph.bitmap.size())
    {
        uint8_t value = glyph.bitmap[symbolIndex++] >> bitshift;

        BitWriter symbol;

        if (!value ||
            (value == whiteValue))
        {
            uint32_t runLength = 1;
            while (symbolIndex < glyph.bitmap.size())
            {
                uint8_t newValue = glyph.bitmap[symbolIndex] >> bitshift;

                if (value != newValue)
                    break;

                symbolIndex++;
                runLength++;
            }

            symbol.writeFixedEncodedValue(value,
                                          encoderSettings.pixelBitNum);

            if (!value)
                symbol.writeRiceEncodedValue(runLength,
                                             encoderSettings.runLengthBlackBitNum);
            else
                symbol.writeRiceEncodedValue(runLength - 1,
                                             encoderSettings.runLengthWhiteBitNum);
        }
        else
            symbol.writeFixedEncodedValue(value,
                                          encoderSettings.pixelBitNum);

        symbols.push_back(symbol);
    }

    // Remove repetitions

    for (int32_t symbolIndex = 0;
         symbolIndex < symbols.size();
         symbolIndex++)
    {
        // Search repetitions

        uint32_t bestRepeatLength;
        uint32_t bestRepeatNum = 1;
        uint32_t bestRepeatBitNum = 0;

        for (uint32_t repeatLength = 2;
             repeatLength < glyph.width;
             repeatLength++)
        {
            uint32_t source_index = symbolIndex;
            uint32_t dest_index = symbolIndex + repeatLength;

            uint32_t repeatNum = 1;
            uint32_t repeatBitNum = 0;

            uint32_t bitNum = 0;

            while (dest_index < symbols.size())
            {
                if (symbols[source_index] != symbols[dest_index])
                    break;

                bitNum += symbols[source_index].getBitNum();

                source_index++;
                dest_index++;

                if (source_index == (symbolIndex + repeatLength))
                {
                    source_index = symbolIndex;
                    repeatNum++;
                    repeatBitNum = bitNum;
                }
            }

            if (repeatBitNum > bestRepeatBitNum)
            {
                bestRepeatLength = repeatLength;
                bestRepeatNum = repeatNum;
                bestRepeatBitNum = repeatBitNum;
            }
        }

        // Replace repetitions

        if (bestRepeatNum > 1)
        {
            uint32_t encodedRepeatLength = bestRepeatLength - 2;
            uint32_t encodedRepeatNum = bestRepeatNum - 2;

            BitWriter symbol;
            symbol.writeFixedEncodedValue(0, encoderSettings.pixelBitNum);
            symbol.writeRiceEncodedValue(0, encoderSettings.runLengthBlackBitNum);
            symbol.writeUnaryEncodedValue(encodedRepeatLength);
            symbol.writeUnaryEncodedValue(encodedRepeatNum);

            uint32_t compressedBitNum = symbol.getBitNum();

            if (compressedBitNum < bestRepeatBitNum)
            {
                symbols.erase(symbols.begin() + symbolIndex +
                                  bestRepeatLength,
                              symbols.begin() + symbolIndex +
                                  bestRepeatLength * bestRepeatNum);

                symbols.insert(symbols.begin() + symbolIndex,
                               symbol);

                symbolIndex += bestRepeatLength;
            }
        }
    }

    // Write glyph data

    BitWriter glyphBitstream;

    glyphBitstream.writeFixedEncodedValue(glyph.left,
                                          encoderSettings.boundingBoxLeftBitNum);
    glyphBitstream.writeFixedEncodedValue(glyph.bottom,
                                          encoderSettings.boundingBoxBottomBitNum);
    glyphBitstream.writeFixedEncodedValue(glyph.width,
                                          encoderSettings.boundingBoxWidthBitNum);
    glyphBitstream.writeFixedEncodedValue(glyph.height,
                                          encoderSettings.boundingBoxHeightBitNum);
    glyphBitstream.writeFixedEncodedValue(glyph.advance,
                                          encoderSettings.advanceBitNum);

    for (BitWriter &symbol : symbols)
        glyphBitstream.write(symbol);

    bitstream.writeVariableLengthWord(glyphBitstream.data.size());
    bitstream.write(glyphBitstream);
}

void encodeFont(Font &font,
                uint32_t pixelBitNum,
                BitWriter &encodedFont)
{
    // Analysis

    uint32_t bestRunLengthBlackBitNum = UINT32_MAX;
    uint32_t bestRunLengthWhiteBitNum = UINT32_MAX;
    uint32_t bestEncodedSize = UINT32_MAX;

    for (uint32_t runLengthBlackBitNum = 0;
         runLengthBlackBitNum < 8;
         runLengthBlackBitNum++)
    {
        for (uint32_t runLengthWhiteBitNum = 0;
             runLengthWhiteBitNum < 8;
             runLengthWhiteBitNum++)
        {
            // Get encoder settings

            EncoderSettings encoderSettings;
            encoderSettings.boundingBoxLeftBitNum =
                getBitNum(font.boundingBoxLeftMinMax) + 1;
            encoderSettings.boundingBoxBottomBitNum =
                getBitNum(font.boundingBoxBottomMinMax) + 1;
            encoderSettings.boundingBoxWidthBitNum =
                getBitNum(font.boundingBoxWidthMinMax);
            encoderSettings.boundingBoxHeightBitNum =
                getBitNum(font.boundingBoxHeightMinMax);
            encoderSettings.advanceBitNum =
                getBitNum(font.advanceMinMax);
            encoderSettings.pixelBitNum = pixelBitNum;
            encoderSettings.runLengthBlackBitNum = runLengthBlackBitNum;
            encoderSettings.runLengthWhiteBitNum = runLengthWhiteBitNum;

            // Write header

            BitWriter bitstream;

            bitstream.writeShort(font.capHeight);         // Font cap height (of uppercase letter A)
            bitstream.writeShort(font.ascent);            // Font ascent (from baseline to top of line)
            bitstream.writeShort(font.descent);           // Font descent (from baseline to bottom of line)
            bitstream.writeShort(font.boundingBoxLeft);   // Font bounding box left
            bitstream.writeShort(font.boundingBoxBottom); // Font bounding box bottom
            bitstream.writeShort(font.boundingBoxWidth);  // Font bounding box width
            bitstream.writeShort(font.boundingBoxHeight); // Font bounding box height

            bitstream.writeByte(encoderSettings.boundingBoxLeftBitNum);   // Bounding box left number of bits
            bitstream.writeByte(encoderSettings.boundingBoxBottomBitNum); // Bounding box bottom number of bits
            bitstream.writeByte(encoderSettings.boundingBoxWidthBitNum);  // Bounding box width number of bits
            bitstream.writeByte(encoderSettings.boundingBoxHeightBitNum); // Bounding box height number of bits
            bitstream.writeByte(encoderSettings.advanceBitNum);           // Advance number of bits
            bitstream.writeByte(pixelBitNum);                             // Pixel number of bits
            bitstream.writeByte(encoderSettings.runLengthBlackBitNum);    // Repeat black number of bits
            bitstream.writeByte(encoderSettings.runLengthWhiteBitNum);    // Repeat white number of bits

            // Write glyphs

            BitWriter blockBitstream;
            Charcode blockCharcode;
            Charcode nextCharcode = -1;

            for (auto &entry : font.glyphs)
            {
                Charcode charcode = entry.first;
                Glyph &glyph = entry.second;

                // Encode block

                if ((charcode != nextCharcode) ||
                    (charcode == 'A') ||
                    (charcode == 'a'))
                {
                    if (nextCharcode != -1)
                    {
                        bitstream.writeVariableLengthWord(blockBitstream.data.size());
                        bitstream.writeVariableLengthWord(blockCharcode);
                        bitstream.write(blockBitstream);

                        blockBitstream.clear();
                    }

                    blockCharcode = charcode;
                }

                nextCharcode = charcode + 1;

                // Encode glyph

                encodeGlyph(glyph, encoderSettings, blockBitstream);
            }

            // Last block

            bitstream.writeVariableLengthWord(blockBitstream.data.size());
            bitstream.writeVariableLengthWord(blockCharcode);
            bitstream.write(blockBitstream);

            bitstream.writeVariableLengthWord(0);

            // Update best

            if (bitstream.getBitNum() < bestEncodedSize)
            {
                bestRunLengthBlackBitNum = runLengthBlackBitNum;
                bestRunLengthWhiteBitNum = runLengthWhiteBitNum;
                bestEncodedSize = bitstream.getBitNum();
                encodedFont = bitstream;
            }
        }
    }
}
