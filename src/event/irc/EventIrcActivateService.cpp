#include "EventIrcActivateService.hpp"
#include "service/irc/IrcService.hpp"
#include "service/irc/IrcConnection.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"

using namespace std;


UUID EventIrcActivateService::getEventUuid() const {
    return this->uuid;
}

EventIrcActivateService::EventIrcActivateService(size_t userId)
    :
    userId{userId}
{
}

std::shared_ptr<EventLoop> EventIrcActivateService::instantiateService(size_t userId, EventQueue* appQueue) const
{
    return make_shared<IrcService>(userId, appQueue);
}

size_t EventIrcActivateService::getUserId() const {
    return userId;
}

IrcServerConfiguration& EventIrcActivateService::addLoginConfiguration(size_t serverId,
                                                                       const std::string& serverName) {
    auto it = loginData.emplace(piecewise_construct,
                                forward_as_tuple(serverId),
                                forward_as_tuple(serverId,
                                                 serverName));
    return it.first->second;
}

const std::map<size_t, IrcServerConfiguration>& EventIrcActivateService::getLoginConfiguration() const {
    return loginData;
}
