#ifndef EVENTHACKHOSTDELETED_H
#define EVENTHACKHOSTDELETED_H

#include "../IClientEvent.hpp"
#include <string>


class EventHackHostDeleted : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string host;
    int port;
public:
    static constexpr UUID uuid = 69;
    virtual UUID getEventUuid() const override;

    EventHackHostDeleted(size_t userId,
                         size_t serverId,
                         const std::string& host,
                         int port);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getHost() const;
    int getPort() const;
};

#endif
