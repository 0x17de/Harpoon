#ifndef EVENTHACKACTIVATEUSER_H
#define EVENTHACKACTIVATEUSER_H

#include "../IActivateServiceEvent.hpp"
#include <string>
#include <map>
#include <list>


class HackServerConfiguration;
class HackChannelLoginData;
class EventHackActivateService : public IActivateServiceEvent {
    size_t userId;
    std::map<size_t, HackServerConfiguration> loginData;
public:
    static constexpr UUID uuid = 47;
    virtual UUID getEventUuid() const override;

    explicit EventHackActivateService(size_t userId);
    virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const override;

    size_t getUserId() const;
    HackServerConfiguration& addLoginConfiguration(size_t serverId,
                                                  const std::string& serverName);
    const std::map<size_t, HackServerConfiguration>& getLoginConfiguration() const;
};

#endif
