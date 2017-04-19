#ifndef FILESYSTEM_IMPL_H
#define FILESYSTEM_IMPL_H

#include <mutex>


class Filesystem_Impl {
public:
	Filesystem_Impl();

	std::recursive_mutex lock;
	std::string root;

    /// Returns the current working directory
	std::string currentDirectory();
    /// Creates a directory with specified access permissions
	bool createDirectory(const std::string& path, mode_t mode);
    /// Changes into a directory
	bool changeDirectory(const std::string& path);
};

#endif
