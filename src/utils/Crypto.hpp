#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>


class Crypto {
public:
	Crypto();
	~Crypto();

	std::string genSalt(size_t size = 32);
	std::string generateKey(const std::string& salt, const std::string& password, size_t keyLength = 64);
	std::string hashPassword(const std::string& salt, const std::string& password, size_t keyLength = 64);
};

#endif
