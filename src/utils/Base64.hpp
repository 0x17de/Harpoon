#ifndef BASE64_H
#define BASE64_H

#include <string>


class Base64 {
public:
	std::string encode(const std::string& source);
	int encodedLength(int length);
};

#endif
