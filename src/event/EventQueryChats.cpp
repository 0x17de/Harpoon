#include "EventQueryChats.hpp"
#include "utils/uuidGen.hpp"


UUID EventQueryChats::uuid = ::uuid.get();
UUID EventQueryChats::getEventUuid() const {
	return this->uuid;
}

EventQueryChats::EventQueryChats(size_t userId, EventQueue* target)
:
	userId{userId},
	target{target}
{
}

size_t EventQueryChats::getUserId() const {
	return userId;
}

EventQueue* EventQueryChats::getTarget() const {
	return target;
}

