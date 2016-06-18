#ifndef INI_H
#define INI_H

#include <map>
#include <string>


class Ini {
	typedef std::map<std::string, std::string> Entries;
	typedef std::map<std::string, Entries> Categories;
	Categories categories;

	std::string filename;
	bool modified;
public:
	Ini(const std::string& filename);
	~Ini();

	void load();
	void write();

	bool getEntry(const std::string& category, const std::string& entry, std::string& data);
	bool setEntry(const std::string& category, const std::string& entry, const std::string& data);
};

#endif
