#ifndef EVENTIRCCHANNELNOTICED_H
#define EVENTIRCCHANNELNOTICED_H

#include "../IEvent.hpp"
#include <string>


class EventIrcChannelNoticed : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string channel;
	std::string message;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcChannelNoticed(size_t userId, size_t serverId,
		const std::string& username,
		const std::string& channel,
		const std::string& message);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getChannel() const;
	std::string getMessage() const;
};

#endif
