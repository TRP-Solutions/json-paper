#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> Base64Decode(const std::string& input) {
    static const int8_t table[256] = {
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
        52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-2,-1,-1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
        15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
        -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
        // rest all -1
    };

    std::vector<uint8_t> output;
    output.reserve((input.size() * 3) / 4);

    int val = 0;
    int valb = -8;

    for (uint8_t c : input) {
        if (c > 127) break;

        int8_t d = table[c];
        if (d == -1) continue;     // skip invalid chars
        if (d == -2) break;        // '=' padding

        val = (val << 6) + d;
        valb += 6;

        if (valb >= 0) {
            output.push_back((uint8_t)((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    return output;
}