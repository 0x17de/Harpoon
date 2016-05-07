#include "Crypto.hpp"
#include "Base64.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>

using namespace std;


static int cryptoCount = 0;

Crypto::Crypto() {
	if (cryptoCount < 1) {
		// Initialize
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL);
	}
	++cryptoCount;
}

Crypto::~Crypto() {
	--cryptoCount;
	if (cryptoCount < 1) {
		// Clean up
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_free_strings();
	}
}

std::string Crypto::genSalt(size_t size) {
	unsigned char bytes[size];
	if (!RAND_bytes(bytes, size))
		throw runtime_error("Can not generate salt.");
	return string(reinterpret_cast<char*>(bytes), size);
}

std::string Crypto::generateKey(const std::string& salt, const std::string& password, size_t keyLength) {
	vector<unsigned char> buffer(keyLength, 0);
	if (!PKCS5_PBKDF2_HMAC(password.c_str(), password.size(),
		reinterpret_cast<const unsigned char*>(salt.c_str()), salt.size(), 40000,
		EVP_sha512(),
		buffer.size(), buffer.data()))
	{
		throw runtime_error("Can not hash password.");
	}
	return string(reinterpret_cast<char*>(buffer.data()), buffer.size());
}

std::string Crypto::hashPassword(const std::string& salt, const std::string& password, size_t keyLength) {
	string key = generateKey(salt, password, keyLength);
	Base64 b;
	vector<char> encodedLength(b.encodedLength(keyLength), 0);
	return b.encode(key);
}

