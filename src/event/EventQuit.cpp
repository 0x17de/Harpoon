#include "EventQuit.h"
#include "utils/uuidGen.h"


UUID EventQuit::uuid = ::uuid.get();
UUID EventQuit::getEventUuid() const {
	return this->uuid;
}

