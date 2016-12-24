#ifndef PASSWORD_H
#define PASSWORD_H

#include <string>


class Password {
    std::string salt;
    std::string hash;
public:
    explicit Password(const std::string& password);
    explicit Password(const std::string& salt, const std::string& password);
    std::string getSaltBase64() const;
    std::string getHashBase64() const;
    bool equals(const std::string& hash);
};

#endif
