#ifndef BASE64_H
#define BASE64_H

#include <string>


class Base64 {
public:
    /// Encode the string as base64
    static std::string encode(const std::string& source);
    /// Returns the length required for the encoded string
    static size_t encodedLength(size_t length);
    /// Decodes a base64 string and returns the result
    static std::string decode(const std::string& source);
    /// Returns the length required for the decoded string
    static size_t decodedLength(const std::string& source);
};

#endif
