#include "EventHackActivateService.hpp"
#include "service/hack/HackService.hpp"
#include "service/hack/HackConnection.hpp"
#include "service/hack/HackChannelLoginData.hpp"
#include "service/hack/HackServerHostConfiguration.hpp"
#include "service/hack/HackServerConfiguration.hpp"

using namespace std;


UUID EventHackActivateService::getEventUuid() const {
    return this->uuid;
}

EventHackActivateService::EventHackActivateService(size_t userId)
    :
    userId{userId}
{
}

std::shared_ptr<EventLoop> EventHackActivateService::instantiateService(size_t userId, EventQueue* appQueue) const
{
    return make_shared<HackService>(userId, appQueue);
}

size_t EventHackActivateService::getUserId() const {
    return userId;
}

HackServerConfiguration& EventHackActivateService::addLoginConfiguration(size_t serverId,
                                                                         const std::string& serverName) {
    auto it = loginData.emplace(piecewise_construct,
                                forward_as_tuple(serverId),
                                forward_as_tuple(serverId,
                                                 serverName));
    return it.first->second;
}

const std::map<size_t, HackServerConfiguration>& EventHackActivateService::getLoginConfiguration() const {
    return loginData;
}
