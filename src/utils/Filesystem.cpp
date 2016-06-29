#include "Filesystem.hpp"
#include "Filesystem_Impl.hpp"
#include <vector>
#include <algorithm>
#include <sstream>

#if defined(__linux__) || defined(__APPLE__)
#ifdef __APPLE__
#pragma warning filesystem support not proven to work on apple platform
#endif
#include <unistd.h>
#include <sys/stat.h>
#elif defined(_WIN32)
#include <windows.h>
#pragma warning filesystem support not proven to work on win32 platform
#else
#error filesystem support yet unavailable for this platform
#endif

using namespace std;


Filesystem::Filesystem()
	: impl{make_shared<Filesystem_Impl>()}
{
}

Filesystem_Impl::Filesystem_Impl()
	: root{currentDirectory()}
{
}

std::string Filesystem_Impl::currentDirectory() {
	lock_guard<recursive_mutex> _lock(lock);

	vector<char> path(200);
	while(true) {
		if (getcwd((char*)path.data(), path.size()) != nullptr)
			break;
		if (errno = ENOMEM)
			path.resize(path.size()*2);
	}

	auto it = find(path.begin(), path.end(), 0);
	size_t end = (it == path.end()) ? path.size() : distance(path.begin(), it);
	return string(path.data(), end);
}

Filesystem& Filesystem::getInstance() {
	static Filesystem filesystem;
	return filesystem;
}

bool Filesystem_Impl::changeDirectory(const std::string& path) {
	lock_guard<recursive_mutex> _lock(lock);

	return chdir(path.c_str()) == 0;
}

bool Filesystem_Impl::createDirectory(const std::string& path, mode_t mode) {
    lock_guard<recursive_mutex> _lock(lock);

    return mkdir(path.c_str(), mode) == 0;
}

bool Filesystem::createPathRecursive(const std::string& path) {
    lock_guard<recursive_mutex> lock(impl->lock);

    if (path.size() == 0) return true;
    impl->changeDirectory((path.at(0) == '/') ? "/" : impl->root);

	istringstream is(path);
	string part;
	while (getline(is, part, '/')) {
		if (part == "") continue;
		if (!impl->changeDirectory(part)) {
			if (!impl->createDirectory(part, 0770)) return false;
			impl->changeDirectory(part);
		}
	}

	return true;
}
