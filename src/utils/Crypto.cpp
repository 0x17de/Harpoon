#include "Crypto.hpp"
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/crypto.h>


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
