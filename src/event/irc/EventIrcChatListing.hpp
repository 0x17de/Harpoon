#ifndef EVENTIRCCHATLISTING_H
#define EVENTIRCCHATLISTING_H

#include "../ISingleClientEvent.hpp"
#include <string>
#include <list>


class EventIrcChatListing : public ISingleClientEvent {
    size_t firstId;
    size_t userId;
    std::list<IrcServerListing> servers;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcChatListing(size_t firstId,
                        size_t userId,
                        void* data);
    IrcServerListing& addServer(std::string activeNick,
                                size_t serverId,
                                std::string serverName);

    size_t getFirstId() const;
    virtual size_t getUserId() const override;
    const std::list<IrcServerListing>& getServerList() const;
    virtual void* getData() const override;
};

#endif
