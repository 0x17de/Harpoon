#ifndef EVENTIRCUSERSTATUSCHANGED_H
#define EVENTIRCUSERSTATUSCHANGED_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>


class EventIrcUserStatusChanged : public IClientEvent, public IrcLoggable {
public:
    enum class Status {
        Joined,
        Parted
    };
private:
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    Status status;
public:
    static constexpr UUID uuid = 34;
    virtual UUID getEventUuid() const override;

    EventIrcUserStatusChanged(size_t userId, size_t serverId, const std::string& username, const std::string& channel, Status status);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    Status getStatus() const;
};

#endif
