#ifndef FILESYSTEM_IMPL_H
#define FILESYSTEM_IMPL_H

#include <mutex>


class Filesystem_Impl {
public:
	Filesystem_Impl();

	std::recursive_mutex lock;
	std::string root;

	std::string currentDirectory();
	bool createDirectory(const std::string& path, mode_t mode);
	bool changeDirectory(const std::string& path);
};

#endif
