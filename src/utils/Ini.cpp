#include "Ini.hpp"
#include <fstream>

using namespace std;


Ini::Ini(const std::string& filename)
:
	filename{filename}
{
	load();
}

Ini::~Ini() {
	if (modified)
		write();
}

void Ini::load() {
	modified = false;
	categories.clear();

	ifstream f(filename);
	if (!f) return;

	Entries* currentCategory;
	string line;
	while(getline(f, line)) {
		if (line.size() > 0 && line.at(line.size()-1) == '\r')
			line = line.substr(0, line.size() - 2);
		if (line.size() == 0) return;

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

void Ini::write() {
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

bool Ini::getEntry(const std::string& category, const std::string& entry, std::string& data) {
	auto it = categories.find(category);
	if (it == categories.end()) return false;
	auto& entries = it->second;
	auto it2 = entries.find(entry);
	if (it2 == entries.end()) return false;
	data = it2->second;
	return true;
}

bool Ini::setEntry(const std::string& category, const std::string& entry, const std::string& data) {
	auto it = categories.find(category);
	Entries* entries;
	if (it == categories.end()) {
		auto res = categories.emplace(piecewise_construct,
			forward_as_tuple(category),
			forward_as_tuple());
		entries = &res.first->second;
	} else {
		entries = &it->second;
	}
	auto it2 = entries->find(entry);
	if (it2 == entries->end()) {
		entries->emplace(piecewise_construct,
			forward_as_tuple(entry),
			forward_as_tuple(data));
	} else {
		it2->second = data;
	}
}
