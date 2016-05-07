#include "EventTemplate.hpp"
#include "utils/uuidGen.hpp"


UUID EventTemplate::uuid = ::uuid.get();
UUID EventTemplate::getEventUuid() const {
	return this->uuid;
}

