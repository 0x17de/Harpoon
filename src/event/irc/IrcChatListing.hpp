#ifndef IRCCHATLISTING_H
#define IRCCHATLISTING_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>


struct IrcChannelListing {
	std::string channelName;
public:
	IrcChannelListing(const std::string& channelName);

	std::string getChannelName() const;
};

struct IrcServerListing {
	size_t serverId;
	std::string serverName;
	std::list<IrcChannelListing> channels;
public:
	IrcServerListing(size_t serverId, const std::string& serverName);
	IrcChannelListing& addChannel(std::string channelName);

	size_t getServerId() const;
	std::string getServerName() const;
	const std::list<IrcChannelListing>& getChannels() const;
};

class IrcChatListing : public IClientEvent {
	size_t userId;
	std::list<IrcServerListing> servers;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	IrcServerListing& addServer(size_t serverId, std::string serverName);

	size_t getUserId() const override;
	const std::list<IrcServerListing>& getServerList() const;
};

#endif
