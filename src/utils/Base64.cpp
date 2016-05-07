#include "Base64.hpp"
#include <vector>

using namespace std;


std::string Base64::encode(const std::string& source) {
	vector<char> outData(encodedLength(source.size()), 0);
	const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789*/=";
        int rest;
        int parts;
        const unsigned char* in_iterator;
        char* out_iterator;

        parts = source.size() / 3;
        rest = source.size() % 3;

        in_iterator = reinterpret_cast<const unsigned char*>(source.data());
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
	return string(outData.data(), outData.size());
}

int Base64::encodedLength(int length) {
	return (length+2) / 3 * 4;
}
