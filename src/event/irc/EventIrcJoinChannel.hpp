#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "../IEvent.hpp"
#include "../IUserEvent.hpp"
#include <list>
#include <string>


struct IrcChannelLoginData {
	IrcChannelLoginData(size_t channelId, const std::string& channel, const std::string& password);
	size_t channelId;
	std::string channel;
	std::string password;
};

class EventIrcJoinChannel : public IUserEvent {
	size_t userId;
	size_t serverId;
	std::list<IrcChannelLoginData> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoinChannel(size_t userId, size_t serverId, std::list<IrcChannelLoginData> loginData);
	virtual size_t getUserId() const override;
	size_t getServerId() const;
	const std::list<IrcChannelLoginData> getLoginData() const;
};

#endif
