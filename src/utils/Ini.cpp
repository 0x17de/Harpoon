#include "Ini.hpp"
#include <fstream>

using namespace std;


Ini::Ini(const std::string& filename)
:
	filename{filename},
	newFile{false}
{
	load();
}

Ini::~Ini() {
	write();
}

void Ini::load() {
	modified = false;
	categories.clear();

	ifstream f(filename);
	if (!f) {
		newFile = true;
		return;
	}

	Entries* currentCategory;
	string line;
	while(getline(f, line)) {
		if (line.size() > 0 && line.at(line.size()-1) == '\r')
			line = line.substr(0, line.size() - 2);
		if (line.size() == 0) continue;

		bool isCategory = line.at(0) == '[';
		if (isCategory) {
			line = line.substr(1, line.size() - 2);
			auto it = categories.emplace(piecewise_construct,
				forward_as_tuple(line),
				forward_as_tuple());
			currentCategory = &it.first->second;
		} else {
			if (!currentCategory) continue;
			size_t assignPosition = line.find('=');
			if (assignPosition == string::npos || line.size() < assignPosition+1) continue;
			currentCategory->emplace(piecewise_construct,
				forward_as_tuple(line.substr(0, assignPosition)),
				forward_as_tuple(line.substr(assignPosition+1)));
		}
	}
}

bool Ini::isNew() const {
	return newFile;
}

void Ini::write(bool bForce) {
	if (!modified && !bForce) return;
	ofstream f(filename);

	bool firstCategory = true;
	for (auto& categoryPair : categories) {
		auto& categoryName = categoryPair.first;
		auto& entries = categoryPair.second;

		if (!firstCategory) f << endl;
		firstCategory = false;

		f << '[' << categoryName << ']' << endl;
		for (auto& entryPair : entries) {
			f << entryPair.first << '=' << entryPair.second << endl;
		}
	}

	modified = false;
}

Ini::Entries& Ini::expectCategory(const std::string& category) {
	auto it = categories.find(category);
	if (it != categories.end())
		return it->second;
	auto res = categories.emplace(piecewise_construct,
		forward_as_tuple(category),
		forward_as_tuple());
	return res.first->second;
}

Ini::Entries* Ini::getEntry(const std::string& category) {
	auto it = categories.find(category);
	if (it == categories.end()) return nullptr;
	return &it->second;
}

bool Ini::getEntry(Entries& entries, const std::string& entry, std::string& data) {
	auto it = entries.find(entry);
	if (it == entries.end()) return false;
	data = it->second;
	return true;
}

bool Ini::getEntry(const std::string& category, const std::string& entry, std::string& data) {
	Entries* entries = getEntry(category);
	if (entries == nullptr) return false;
	return getEntry(*entries, entry, data);
}

void Ini::setEntry(const std::string& category, const std::string& entry, const std::string& data) {
	setEntry(expectCategory(category), entry, data);
}

void Ini::setEntry(Entries& entries, const std::string& entry, const std::string& data) {
	auto it = entries.find(entry);
	if (it == entries.end()) {
		entries.emplace(piecewise_construct,
			forward_as_tuple(entry),
			forward_as_tuple(data));
	} else {
		it->second = data;
	}
	modified = true;
}
