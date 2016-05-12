#include "EventTemplate.hpp"
#include "utils/uuidGen.hpp"


UUID EventTemplate::uuid = ::uuid.get();
UUID EventTemplate::getEventUuid() const {
	return this->uuid;
}

EventTemplate::EventTemplate(size_t userId, size_t serverId)
:
	userId{userId},
	serverId{serverId}
{
}

size_t EventTemplate::getUserId() const {
	return userId;
}

size_t EventTemplate::getServerId() const {
	return serverId;
}

