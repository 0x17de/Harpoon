#ifndef BASE64_H
#define BASE64_H

#include <string>


class Base64 {
public:
    static std::string encode(const std::string& source);
    static size_t encodedLength(size_t length);
    static std::string decode(const std::string& source);
    static size_t decodedLength(const std::string& source);
};

#endif
