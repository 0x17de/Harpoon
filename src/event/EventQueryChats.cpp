#include "EventQueryChats.hpp"
#include "utils/uuidGen.hpp"


UUID EventQueryChats::uuid = ::uuid.get();
UUID EventQueryChats::getEventUuid() const {
	return this->uuid;
}

EventQueryChats::EventQueryChats(size_t userId, EventQueue* target, void* data)
:
	userId{userId},
	target{target},
	data{data}
{
}

size_t EventQueryChats::getUserId() const {
	return userId;
}

EventQueue* EventQueryChats::getTarget() const {
	return target;
}

void* EventQueryChats::getData() const {
	return data;
}

