#ifndef EVENTIRCNUMERIC_H
#define EVENTIRCNUMERIC_H

#include "../IEvent.hpp"
#include <string>
#include <vector>


class EventIrcNumeric : public IEvent {
	size_t userId;
	size_t serverId;
	unsigned int eventCode;
	std::string from;
	std::vector<std::string> parameters;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcNumeric(size_t userId, size_t serverId,
		unsigned int eventCode,
		std::string from,
		std::vector<std::string> parameters);
	size_t getUserId() const;
	size_t getServerId() const;
	unsigned int getEventCode() const;
	std::string getFrom() const;
	const std::vector<std::string>& getParameters() const;
};

#endif
