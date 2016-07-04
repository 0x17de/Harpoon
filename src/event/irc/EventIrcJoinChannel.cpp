#include "EventIrcJoinChannel.hpp"
#include "utils/uuidGen.hpp"
#include "service/irc/IrcChannelLoginData.hpp"


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
std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() {
    return loginData;
}
const std::list<IrcChannelLoginData> EventIrcJoinChannel::getLoginData() const {
    return loginData;
}

