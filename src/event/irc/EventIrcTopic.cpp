#include "EventIrcTopic.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcTopic::uuid = ::uuid.get();
UUID EventIrcTopic::getEventUuid() const {
	return this->uuid;
}

EventIrcTopic::EventIrcTopic(size_t userId, size_t serverId, const std::string& username, const std::string& channel, const std::string& topic)
:
	userId{userId},
	serverId{serverId},
	username{username},
	channel{channel},
	topic{topic}
{
}

size_t EventIrcTopic::getUserId() const {
	return userId;
}

size_t EventIrcTopic::getServerId() const {
	return serverId;
}

std::string EventIrcTopic::getUsername() const {
	return username;
}

std::string EventIrcTopic::getChannel() const {
	return channel;
}

std::string EventIrcTopic::getTopic() const {
	return topic;
}

