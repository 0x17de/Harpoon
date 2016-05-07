#include "EventLogin.hpp"
#include "utils/uuidGen.hpp"


UUID EventLogin::uuid = ::uuid.get();
UUID EventLogin::getEventUuid() const {
	return this->uuid;
}

