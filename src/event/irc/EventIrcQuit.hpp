#ifndef EVENTIRCQUIT_H
#define EVENTIRCQUIT_H

#include "../IEvent.hpp"
#include "../IUserEvent.hpp"
#include <cstdlib>
#include <string>


class EventIrcQuit : public IUserEvent {
	size_t userId;
	size_t serverId;
	std::string who;
	std::string reason;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcQuit(size_t userId, size_t serverId, const std::string& who, const std::string& reason);
	size_t getUserId() const override;
	size_t getServerId() const;
	std::string getWho() const;
	std::string getReason() const;
};

#endif
