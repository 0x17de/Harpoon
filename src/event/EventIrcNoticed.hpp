#ifndef EVENTIRCNOTICED_H
#define EVENTIRCNOTICED_H

#include "IEvent.hpp"
#include <string>


class EventIrcNoticed : public IEvent {
	size_t userId;
	size_t serverId;
	std::string username;
	std::string target;
	std::string message;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcNoticed(size_t userId, size_t serverId,
		const std::string& username,
		const std::string& target,
		const std::string& message);
	size_t getUserId() const;
	size_t getServerId() const;
	std::string getUsername() const;
	std::string getTarget() const;
	std::string getMessage() const;
};

#endif
