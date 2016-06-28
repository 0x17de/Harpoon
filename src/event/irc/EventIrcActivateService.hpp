#ifndef EVENTIRCACTIVATEUSER_H
#define EVENTIRCACTIVATEUSER_H

#include "../IActivateServiceEvent.hpp"
#include <string>
#include <map>
#include <list>


class IrcServerConfiguration;
class IrcChannelLoginData;
class EventIrcActivateService : public IActivateServiceEvent {
    size_t userId;
    std::map<size_t, IrcServerConfiguration> loginData;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcActivateService(size_t userId);
    virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const override;

    size_t getUserId() const;
    IrcServerConfiguration& addLoginConfiguration(size_t serverId,
                                                  const std::string& serverName);
    const std::map<size_t, IrcServerConfiguration>& getLoginConfiguration() const;
};

#endif
