#ifndef BASE64_H
#define BASE64_H

#include <string>


class Base64 {
public:
    std::string encode(const std::string& source);
    size_t encodedLength(size_t length);
    std::string decode(const std::string& source);
    size_t decodedLength(const std::string& source);
};

#endif
