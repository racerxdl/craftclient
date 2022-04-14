#pragma once

#include <cstdint>

namespace ProtoRock {
    /**
     * Encodes an unsigned variable-length integer using the MSB algorithm.
     * This function assumes that the value is stored as little endian.
     * @param value The input value. Any standard integer type is allowed.
     * @param output A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
     * @return The number of bytes used in the output memory.
     */
    template<typename int_t = uint64_t>
    std::size_t encodeVarint(int_t value, uint8_t* output) {
        std::size_t outputSize = 0;
        // While more than 7 bits of data are left, occupy the last output byte
        // and set the next byte flag
        while (value > 127) {
            //|128: Set the next byte flag
            output[outputSize] = ((uint8_t)(value & 127)) | 128;
            //Remove the seven bits we just wrote
            value >>= 7;
            outputSize++;
        }
        output[outputSize++] = ((uint8_t)value) & 127;
        return outputSize;
    }

    /**
     * Decodes an unsigned variable-length integer using the MSB algorithm.
     * @param value A variable-length encoded integer of arbitrary size.
     * @param inputSize How many bytes are
     */
    template<typename int_t = uint64_t>
    int_t decodeVarint(uint8_t* input, std::size_t inputSize) {
        int_t ret = 0;
        for (std::size_t i = 0; i < inputSize; i++) {
            ret |= (input[i] & 127) << (7 * i);
            //If the next-byte flag is set
            if(!(input[i] & 128)) {
                break;
            }
        }
        return ret;
    }
}