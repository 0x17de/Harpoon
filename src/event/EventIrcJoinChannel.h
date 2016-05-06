#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "IEvent.h"
#include <list>
#include <string>


struct IrcChannelLoginData {
	size_t channelId;
	std::string channel;
	std::string password;
};

class EventIrcJoinChannel : public IEvent {
	size_t serverId;
	std::list<IrcChannelLoginData> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoinChannel(size_t serverId, std::list<IrcChannelLoginData> loginData);
	size_t getServerId() const;
	const std::list<IrcChannelLoginData> getLoginData() const;
};

#endif
