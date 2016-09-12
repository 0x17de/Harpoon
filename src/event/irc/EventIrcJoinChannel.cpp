#include "EventIrcJoinChannel.hpp"
#include "utils/uuidGen.hpp"
#include "service/irc/IrcChannelLoginData.hpp"


IrcChannelJoinData::IrcChannelJoinData(const std::string& name)
    : name{name}
    , passwordSpecified{false}
{
}

IrcChannelJoinData::IrcChannelJoinData(const std::string& name, const std::string& password)
    : name{name}
    , passwordSpecified{true}
    , password{password}
{
}

EventIrcJoinChannel::EventIrcJoinChannel(size_t userId, size_t serverId)
    : userId{userId}
    , serverId{serverId}
{
}

UUID EventIrcJoinChannel::uuid = ::uuid.get();
UUID EventIrcJoinChannel::getEventUuid() const {
    return this->uuid;
}

size_t EventIrcJoinChannel::getUserId() const {
    return userId;
}

size_t EventIrcJoinChannel::getServerId() const {
    return serverId;
}

void EventIrcJoinChannel::addLoginData(const std::string& name, const std::string& password) {
    return loginData.emplace_back(name, password);
}

const std::list<IrcChannelJoinData>& EventIrcJoinChannel::getLoginData() const {
    return loginData;
}

