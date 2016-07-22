#ifndef EVENTIRCPARTCHANNEL_H
#define EVENTIRCPARTCHANNEL_H

#include "IIrcCommand.hpp"
#include <list>
#include <string>


class EventIrcPartChannel : public IIrcCommand {
	size_t userId;
	size_t serverId;
	std::list<std::string> channels;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcPartChannel(size_t userId, size_t serverId);
	virtual size_t getUserId() const override;
	virtual size_t getServerId() const override;
    void addChannel(const std::string& channel);
	const std::list<std::string> getChannels() const;
};

#endif
