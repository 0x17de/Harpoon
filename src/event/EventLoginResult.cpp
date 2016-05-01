#include "EventLoginResult.h"
#include "utils/uuidGen.h"


UUID EventLoginResult::uuid = ::uuid.get();
UUID EventLoginResult::getEventUuid() const {
	return this->uuid;
}

