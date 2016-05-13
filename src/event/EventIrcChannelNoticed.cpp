#include "EventIrcChannelNoticed.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcChannelNoticed::uuid = ::uuid.get();
UUID EventIrcChannelNoticed::getEventUuid() const {
	return this->uuid;
}

EventIrcChannelNoticed::EventIrcChannelNoticed(size_t userId, size_t serverId,
	const std::string& username,
	const std::string& channel,
	const std::string& message)
:
	userId{userId},
	serverId{serverId},
	username{username},
	channel{channel},
	message{message}
{
}

size_t EventIrcChannelNoticed::getUserId() const {
	return userId;
}

size_t EventIrcChannelNoticed::getServerId() const {
	return serverId;
}

std::string EventIrcChannelNoticed::getUsername() const {
	return username;
}

std::string EventIrcChannelNoticed::getChannel() const {
	return channel;
}

std::string EventIrcChannelNoticed::getMessage() const {
	return message;
}

