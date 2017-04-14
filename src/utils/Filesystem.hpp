#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <memory>


class Filesystem_Impl;
class Filesystem {
	std::shared_ptr<Filesystem_Impl> impl;
    Filesystem();
public:
    static Filesystem& getInstance();
    /// Recursively creates the required direcory in the current working directory
	bool createPathRecursive(const std::string& path);
};

#endif
