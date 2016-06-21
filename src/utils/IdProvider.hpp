#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include "utils/Ini.hpp"


class IdProvider {
	Ini idIni;
	Ini::Entries& idMap;
	bool modified;
	IdProvider();
public:
	static IdProvider& getInstance();
	size_t generateNewId(const std::string& key);
	void save();
};

#endif
