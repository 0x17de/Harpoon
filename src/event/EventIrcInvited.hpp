#ifndef EVENTIRCINVITED_H
#define EVENTIRCINVITED_H

#include "IEvent.hpp"
#include <string>


class EventIrcInvited : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string target;
	std::string channel;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcInvited(size_t userId, size_t serverId,
		const std::string& username,
		const std::string& target,
		const std::string& channel);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getTarget() const;
	std::string getChannel() const;
};

#endif
