#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "IEvent.h"
#include <list>
#include <string>


class IrcChannelLoginData {
	std::string username;
	std::string password;
};

class EventIrcJoinChannel : public IEvent {
	std::list<IrcChannelLoginData> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoinChannel(std::list<IrcChannelLoginData> loginData);
};

#endif
