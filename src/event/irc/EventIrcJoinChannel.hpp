#ifndef EVENTIRCJOINCHANNEL_H
#define EVENTIRCJOINCHANNEL_H

#include "../IEvent.hpp"
#include "../IUserEvent.hpp"
#include <list>
#include <string>


class IrcChannelLoginData {
	size_t channelId;
	std::string channelName;
	std::string channelPassword;
public:
	IrcChannelLoginData(size_t channelId, const std::string& channel, const std::string& channelPassword);

	size_t getChannelId() const;
	std::string getChannelName() const;
	std::string getChannelPassword() const;
};

class EventIrcJoinChannel : public IUserEvent {
	size_t userId;
	size_t serverId;
	std::list<IrcChannelLoginData> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcJoinChannel(size_t userId, size_t serverId);
	virtual size_t getUserId() const override;
	size_t getServerId() const;
	std::list<IrcChannelLoginData> getLoginData();
	const std::list<IrcChannelLoginData> getLoginData() const;
};

#endif
