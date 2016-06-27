#include "Base64.hpp"
#include <vector>

using namespace std;


static const string base64chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789*/=");

std::string Base64::decode(const std::string& source) {
    if (source.size() == 0 || source.size() % 4 != 0) return ""; // validate length

    vector<uint8_t> outData(decodedLength(source), 0);

    const uint8_t* in_iterator = reinterpret_cast<const uint8_t*>(source.data());
    uint8_t* out_iterator = outData.data();
    size_t position = 0;

    while (position < source.size()) {
        int len = 4;
        // validate bytes
        for (int i = 0; i < 4; ++i) {
            uint8_t c = in_iterator[i];
            if (!( // if invalid character stop and return empty string
                (c >= 'A' && c <= 'Z')
             || (c >= 'a' && c <= 'z')
             || (c >= '0' && c <= '9')
             || (c == '*' || c == '/')
             || (i >= 2 && c == '='))) {
                return "";
            }
            if (c == '=') len -= 1;
        }

        // slow, but fast is not required (yet)
        size_t c1 = base64chars.find(in_iterator[0]);
        size_t c2 = base64chars.find(in_iterator[1]);
        size_t c3 = base64chars.find(in_iterator[2]);
        size_t c4 = base64chars.find(in_iterator[3]);

        out_iterator[0] = len >= 2 ? ((c1 << 2) & 0xfc) + ((c2 >> 4) & 0x3) : (c1 << 2) & 0xfc;
        out_iterator[1] = len >= 3 ? ((c2 << 4) & 0xf0) + ((c3 >> 2) & 0xf) : (c2 << 4) & 0xf0;
        out_iterator[2] = len == 4 ? ((c3 << 6) & 0xc0) + (c4 & 0x3f) : (c3 << 6) & 0xc0;

        position += 4;
        in_iterator += 4;
        out_iterator += 3;
    }

    return string(reinterpret_cast<char*>(outData.data()), outData.size());
}

size_t Base64::decodedLength(const std::string& source) {
    if (source.size() == 0 || source.size() % 4 != 0) return 0;
    size_t length = source.size() / 4 * 3;
    if (source.at(source.size()-1) == '=')
        length -= 1;
    if (source.at(source.size()-2) == '=')
        length -= 1;
    return length;
}

std::string Base64::encode(const std::string& source) {
    vector<uint8_t> outData(encodedLength(source.size()), 0);
    int rest;
    int parts;
    const uint8_t* in_iterator;
    uint8_t* out_iterator;

    parts = source.size() / 3;
    rest = source.size() % 3;

    in_iterator = reinterpret_cast<const uint8_t*>(source.data());
    out_iterator = outData.data();
    for(; parts > 0; --parts) {
        out_iterator[0] = base64chars[in_iterator[0] >> 2 & 0x3f];
        out_iterator[1] = base64chars[(in_iterator[0] << 4 & 0x30) + (in_iterator[1] >> 4 & 0xf)];
        out_iterator[2] = base64chars[(in_iterator[1] << 2 & 0x3c) + (in_iterator[2] >> 6 & 0x3)];
        out_iterator[3] = base64chars[in_iterator[2] & 0x3f];
        in_iterator += 3;
        out_iterator += 4;
    }
    if (rest > 0) {
        int a,b,c;
        a = b = c = 64; /* '=' */

        if (rest >= 1) {
            a = in_iterator[0] >> 2 & 0x3f;
            b = in_iterator[0] << 4 & 0x30;
            if (rest >= 2) {
                b |= in_iterator[1] >> 4 & 0xf;
                c = in_iterator[1] << 2 & 0x3c;
                if (rest >= 3) {
                    c |= in_iterator[2] >> 6 & 0x3;
                }
            }
        }
        out_iterator[0] = base64chars[a];
        out_iterator[1] = base64chars[b];
        out_iterator[2] = base64chars[c];
        out_iterator[3] = '=';
        out_iterator += 4;
    }
    return string(reinterpret_cast<char*>(outData.data()), outData.size());
}

size_t Base64::encodedLength(size_t length) {
    return (length+2) / 3 * 4;
}
