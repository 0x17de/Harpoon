#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>


class Filesystem_Impl;
class Filesystem {
	std::shared_ptr<Filesystem_Impl> impl;
    Filesystem();
public:
    static Filesystem& getInstance();
	bool createPathRecursive(const std::string& path);
};

#endif
