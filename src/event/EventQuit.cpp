#include "EventQuit.hpp"
#include "utils/uuidGen.hpp"


UUID EventQuit::uuid = ::uuid.get();
UUID EventQuit::getEventUuid() const {
	return this->uuid;
}

