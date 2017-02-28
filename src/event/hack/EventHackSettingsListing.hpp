#ifndef EVENTHACKSETTINGSLISTING_H
#define EVENTHACKSETTINGSLISTING_H

#include "../ISingleClientEvent.hpp"
#include "service/hack/HackServerConfiguration.hpp"
#include <string>
#include <list>


class EventHackSettingsListing : public ISingleClientEvent {
    size_t userId;
	std::list<HackServerConfiguration> servers;
    void* data;
public:
    static constexpr UUID uuid = 67;
    virtual UUID getEventUuid() const override;

    EventHackSettingsListing(size_t userId, void* data);
    HackServerConfiguration& addServer(size_t serverId, std::string serverName);

    virtual size_t getUserId() const override;
    virtual void* getData() const override;
    const std::list<HackServerConfiguration>& getServerList() const;
};

#endif
