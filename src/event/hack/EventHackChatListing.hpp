#ifndef EVENTHACKCHATLISTING_H
#define EVENTHACKCHATLISTING_H

#include "../ISingleClientEvent.hpp"
#include "HackServerListing.hpp"
#include <string>
#include <list>


class EventHackChatListing : public ISingleClientEvent {
    size_t firstId;
    size_t userId;
    std::list<HackServerListing> servers;
    void* data;
public:
    static constexpr UUID uuid = 49;
    virtual UUID getEventUuid() const override;

    EventHackChatListing(size_t firstId,
                         size_t userId,
                         void* data);
    HackServerListing& addServer(std::string activeNick,
                                 size_t serverId,
                                 std::string serverName);

    size_t getFirstId() const;
    virtual size_t getUserId() const override;
    const std::list<HackServerListing>& getServerList() const;
    virtual void* getData() const override;
};

#endif
