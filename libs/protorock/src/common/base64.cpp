#include "common/base64.h"


std::vector<int> buildCharMap() {
    std::vector<int> lut(256, -1);
    for (int i = 0; i < 64; i++) {
        lut[ProtoRock::Base64::Alphabet[i]] = i;
    }
    return lut;
}

const std::vector<int> ProtoRock::Base64::CharMap = buildCharMap();