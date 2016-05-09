#include "EventIrcMessage.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcMessage::uuid = ::uuid.get();
UUID EventIrcMessage::getEventUuid() const {
	return this->uuid;
}

EventIrcMessage::EventIrcMessage(std::string from, std::string channel, std::string message)
:
	from{from},
	channel{channel},
	message{message}
{
}

std::string EventIrcMessage::getFrom() const {
	return from;
}

std::string EventIrcMessage::getChannel() const {
	return channel;
}

std::string EventIrcMessage::getMessage() const {
	return message;
}

