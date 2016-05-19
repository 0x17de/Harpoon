#include "EventLoginResult.hpp"
#include "utils/uuidGen.hpp"


UUID EventLoginResult::uuid = ::uuid.get();
UUID EventLoginResult::getEventUuid() const {
	return this->uuid;
}

EventLoginResult::EventLoginResult(bool success, size_t userId, void* data)
:
	success{success},
	userId{userId},
	data{data}
{
}

bool EventLoginResult::getSuccess() const {
	return success;
}

size_t EventLoginResult::getUserId() const {
	return userId;
}

void* EventLoginResult::getData() const {
	return data;
}

