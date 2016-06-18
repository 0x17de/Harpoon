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
	bool newFile;
public:
	Ini(const std::string& filename);
	~Ini();

	void load();
	void write();

	bool isNew() const;

	Entries* getEntry(const std::string& category);
	Entries& expectCategory(const std::string& category);
	bool getEntry(const std::string& category, const std::string& entry, std::string& data);
	bool getEntry(Entries& entries, const std::string& entry, std::string& data);
	void setEntry(const std::string& category, const std::string& entry, const std::string& data);
	void setEntry(Entries& entries, const std::string& entry, const std::string& data);
};

#endif
