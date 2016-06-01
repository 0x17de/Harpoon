#include "IrcChatListing.hpp"
#include "utils/uuidGen.hpp"


UUID IrcChatListing::uuid = ::uuid.get();
UUID IrcChatListing::getEventUuid() const {
	return this->uuid;
}

IrcChannelListing::IrcChannelListing(const std::string& channelName)
:
	channelName{channelName}
{
}

std::string IrcChannelListing::getChannelName() const {
	return channelName;
}

IrcServerListing::IrcServerListing(size_t serverId, const std::string& serverName)
:
	serverId{serverId},
	serverName{serverName}
{
}

IrcChannelListing& IrcServerListing::addChannel(std::string channelName) {
	channels.emplace_back(channelName);
	return channels.back();
}

size_t IrcServerListing::getServerId() const {
	return serverId;
}

std::string IrcServerListing::getServerName() const {
	return serverName;
}

const std::list<IrcChannelListing>& IrcServerListing::getChannels() const {
	return channels;
}

IrcServerListing& IrcChatListing::addServer(size_t serverId, std::string serverName) {
	servers.emplace_back(serverId, serverName);
	return servers.back();
}

size_t IrcChatListing::getUserId() const {
	return userId;
}

const std::list<IrcServerListing>& IrcChatListing::getServerList() const {
	return servers;
}

