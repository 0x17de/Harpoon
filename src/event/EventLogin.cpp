#include "EventLogin.h"
#include "utils/uuidGen.h"


UUID EventLogin::uuid = ::uuid.get();
UUID EventLogin::getEventUuid() const {
	return this->uuid;
}

