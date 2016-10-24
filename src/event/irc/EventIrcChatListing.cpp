#include "EventIrcChatListing.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcChatListing::uuid = ::uuid.get();
UUID EventIrcChatListing::getEventUuid() const {
    return this->uuid;
}

IrcChannelUser::IrcChannelUser(const std::string& nick,
                               const std::string& mode)
    : nick{nick}
    , mode{mode}
{
}

std::string IrcChannelUser::getNick() const {
    return nick;
}

std::string IrcChannelUser::getMode() const {
    return mode;
}

IrcChannelListing::IrcChannelListing(const std::string& channelName,
                                     const std::string& channelTopic,
                                     bool disabled)
    : channelName{channelName}
    , channelTopic{channelTopic}
    , disabled{disabled}
{
}

void IrcChannelListing::addUser(const std::string& nick, const std::string& mode) {
    users.emplace_back(nick, mode);
}

const std::list<IrcChannelUser>& IrcChannelListing::getUsers() const {
    return users;
}


std::string IrcChannelListing::getChannelName() const {
    return channelName;
}

std::string IrcChannelListing::getChannelTopic() const {
    return channelTopic;
}

bool IrcChannelListing::getDisabled() const {
    return disabled;
}

IrcServerListing::IrcServerListing(std::string activeNick,
                                   size_t serverId,
                                   const std::string& serverName)
    : activeNick{activeNick}
    , serverId{serverId}
    , serverName{serverName}
{
}

IrcChannelListing& IrcServerListing::addChannel(const std::string& channelName,
                                                const std::string& channelTopic,
                                                bool disabled) {
    channels.emplace_back(channelName,
                          channelTopic,
                          disabled);
    return channels.back();
}

std::string IrcServerListing::getActiveNick() const {
    return activeNick;
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

EventIrcChatListing::EventIrcChatListing(size_t firstId,
                                         size_t userId,
                                         void* data)
    : firstId{firstId}
    , userId{userId}
    , data{data}
{
}

IrcServerListing& EventIrcChatListing::addServer(std::string activeNick,
                                                 size_t serverId,
                                                 std::string serverName) {
    servers.emplace_back(activeNick, serverId, serverName);
    return servers.back();
}

size_t EventIrcChatListing::getFirstId() const {
    return firstId;
}

size_t EventIrcChatListing::getUserId() const {
    return userId;
}

const std::list<IrcServerListing>& EventIrcChatListing::getServerList() const {
    return servers;
}

void* EventIrcChatListing::getData() const {
    return data;
}

