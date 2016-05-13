#ifndef EVENTIRCMODECHANGED_H
#define EVENTIRCMODECHANGED_H

#include "IEvent.hpp"
#include <string>


class EventIrcModeChanged : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string channel;
	std::string mode;
	std::string arg;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcModeChanged(size_t userId, size_t serverId,
		const std::string& username,
		const std::string& channel,
		const std::string& mode,
		const std::string& arg);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getChannel() const;
	std::string getMode() const;
	std::string getArg() const;
};

#endif
