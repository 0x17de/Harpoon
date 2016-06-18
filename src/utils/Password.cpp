#include "Password.hpp"
#include "utils/Crypto.hpp"
#include "utils/Base64.hpp"


Password::Password(const std::string& password) {
	Crypto c;
	salt = c.genSalt();
	hash = c.hashPassword(salt, password);
}
Password::Password(const std::string& lsalt, const std::string& password) {
	Crypto c;
	salt = Base64().decode(lsalt);
	hash = c.hashPassword(salt, password);
}
std::string Password::getSaltBase64() const {
	return Base64().encode(salt);
}
std::string Password::getHashBase64() const {
	return hash;
}
bool Password::equals(const std::string& lhash) {
	return hash == lhash;
}
