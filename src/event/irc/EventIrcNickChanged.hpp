#ifndef EVENTIRCNICKCHANGED_H
#define EVENTIRCNICKCHANGED_H

#include "../IEvent.hpp"
#include <string>


class EventIrcNickChanged : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string newNick;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcNickChanged(size_t userId, size_t serverId, const std::string& username, const std::string& newNick);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getNewNick() const;
};

#endif
