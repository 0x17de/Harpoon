#ifndef EVENTIRCCHATLISTING_H
#define EVENTIRCCHATLISTING_H

#include "../ISingleClientEvent.hpp"
#include <string>
#include <list>


class IrcChannelUser {
	std::string nick;
	std::string mode;
public:
	IrcChannelUser(const std::string& nick, const std::string& mode);

	std::string getNick() const;
	std::string getMode() const;
};

class IrcChannelListing {
	std::string channelName;
	std::list<IrcChannelUser> users;
public:
	IrcChannelListing(const std::string& channelName);

	void addUser(const std::string& nick, const std::string& mode);
	const std::list<IrcChannelUser>& getUsers() const;
	std::string getChannelName() const;
};

class IrcServerListing {
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

class EventIrcChatListing : public ISingleClientEvent {
	size_t userId;
	std::list<IrcServerListing> servers;
	void* data;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcChatListing(size_t userId, void* data);
	IrcServerListing& addServer(size_t serverId, std::string serverName);

	size_t getUserId() const override;
	const std::list<IrcServerListing>& getServerList() const;
	void* getData() const;
};

#endif
