#ifndef EVENTIRCUSERSTATUSCHANGED_H
#define EVENTIRCUSERSTATUSCHANGED_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>


/// Event for notifying that a irc user's status changed
class EventIrcUserStatusChanged : public IClientEvent, public IrcLoggable {
public:
    enum class Status {
        Joined,
        Parted,
        Kicked,
        Quit,
        Mode
    };
private:
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    Status status;
    std::string targetOrMode;
    std::string reason;
public:
    static constexpr UUID uuid = 34;
    virtual UUID getEventUuid() const override;

    EventIrcUserStatusChanged(size_t userId,
                              size_t serverId,
                              Status status,
                              const std::string& username,
                              const std::string& channel = "",
                              const std::string& targetOrMode = "",
                              const std::string& reason = "");
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    Status getStatus() const;
    std::string getReason() const;
    std::string getTarget() const;
    std::string getMode() const;
};

#endif
