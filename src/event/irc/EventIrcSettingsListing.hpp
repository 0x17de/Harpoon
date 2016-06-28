#ifndef EVENTIRCSETTINGSLISTING_H
#define EVENTIRCSETTINGSLISTING_H

#include "../ISingleClientEvent.hpp"
#include <string>
#include <list>


class IrcServerConfiguration;
class EventIrcSettingsListing : public ISingleClientEvent {
    size_t userId;
	std::list<IrcServerConfiguration> servers;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcSettingsListing(size_t userId, void* data);
    IrcServerConfiguration& addServer(size_t serverId, std::string serverName);

    size_t getUserId() const override;
    const std::list<IrcServerConfiguration>& getServerList() const;
    void* getData() const;
};

#endif
