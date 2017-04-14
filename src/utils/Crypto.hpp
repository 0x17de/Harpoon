#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>


class Crypto {
public:
    Crypto();
    ~Crypto();

    /// Generates some salt for password hashing
    std::string genSalt(size_t size = 32);
    /// Generates a key from fhe password
    std::string generateKey(const std::string& salt, const std::string& password, size_t keyLength = 64);
    /// Returns a base64 encoded key
    std::string hashPassword(const std::string& salt, const std::string& password, size_t keyLength = 64);
};

#endif
