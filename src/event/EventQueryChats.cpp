#include "EventQueryChats.hpp"
#include "utils/uuidGen.hpp"


UUID EventQueryChats::uuid = ::uuid.get();
UUID EventQueryChats::getEventUuid() const {
	return this->uuid;
}

EventQueryChats::EventQueryChats(size_t userId, void* data)
:
	userId{userId},
	target{target},
	data{data}
{
}

size_t EventQueryChats::getUserId() const {
	return userId;
}

void* EventQueryChats::getData() const {
	return data;
}

