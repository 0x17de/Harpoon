#include "EventLoginResult.hpp"
#include "utils/uuidGen.hpp"


UUID EventLoginResult::uuid = ::uuid.get();
UUID EventLoginResult::getEventUuid() const {
	return this->uuid;
}

