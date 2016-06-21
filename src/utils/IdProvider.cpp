#include "IdProvider.hpp"
#include <string>
#include <sstream>

using namespace std;


IdProvider::IdProvider()
:
	idIni{"config/ids.ini"},
	idMap{idIni.expectCategory("ids")}
{
}

IdProvider& IdProvider::getInstance() {
	static IdProvider idProvider;
	return idProvider;
}

size_t IdProvider::generateNewId(const std::string& entry) {
	string entryIdString;
	size_t entryId;
	if (idIni.getEntry(idMap, entry, entryIdString)) {
		istringstream(entryIdString) >> entryId;
	} else {
		entryId = 0;
	}

	entryId += 1;
	idIni.setEntry(idMap, entry, to_string(entryId));

	return entryId;
}

void IdProvider::save() {
	idIni.write();
}
