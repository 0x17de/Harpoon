#ifndef PASSWORD_H
#define PASSWORD_H

#include <string>


/// Wrapper for simplified secure password hashing
class Password {
    std::string salt;
    std::string hash;
public:
    /// Hashes some password with a random salt
    explicit Password(const std::string& password);
    /// Hashes some password with the provided salt
    explicit Password(const std::string& salt, const std::string& password);
    /// Returns the used salt as a base64 encoded string
    std::string getSaltBase64() const;
    /// Returns the password hash as a base64 encoded string
    std::string getHashBase64() const;
    /// True if both password hashes are the same
    bool equals(const std::string& hash) const;
};

#endif
