#include "EventActivateUser.hpp"
#include "utils/uuidGen.hpp"


UUID EventActivateUser::uuid = ::uuid.get();
UUID EventActivateUser::getEventUuid() const {
	return this->uuid;
}

