#include "EventInit.hpp"
#include "utils/uuidGen.hpp"


UUID EventInit::uuid = ::uuid.get();
UUID EventInit::getEventUuid() const {
	return this->uuid;
}

