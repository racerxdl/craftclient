#pragma once

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace ProtoRock {

namespace Base64 {
const static char *Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

extern const std::vector<int> CharMap;

inline std::string Encode(const std::vector<uint8_t> &in) {
    std::string out;

    int val = 0, valb = -6;
    for (auto c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(Alphabet[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        out.push_back(Alphabet[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (out.size() % 4) {
        out.push_back('=');
    }

    return out;
}

inline std::string Encode(const std::string &in) {
    return Encode(std::vector<uint8_t>(in.begin(), in.end()));
}

inline std::vector<uint8_t> Decode(const std::string &in) {
    std::vector<uint8_t> out;

    int val = 0, valb = -8;
    int p = 0;
    for (auto c : in) {
        if (CharMap[c] == -1) {
            break;
        }
        p++;
        val = (val << 6) + CharMap[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return out;
}
}  // namespace Base64

}  // namespace ProtoRock