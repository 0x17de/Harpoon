#ifndef EVENTIRCTOPIC_H
#define EVENTIRCTOPIC_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>


class EventIrcTopic : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    std::string topic;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcTopic(size_t userId, size_t serverId, const std::string& username, const std::string& channel, const std::string& topic);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    std::string getTopic() const;
};

#endif
