#include "EventTemplate.h"
#include "utils/uuidGen.h"


UUID EventTemplate::uuid = ::uuid.get();
UUID EventTemplate::getEventUuid() const {
	return this->uuid;
}

