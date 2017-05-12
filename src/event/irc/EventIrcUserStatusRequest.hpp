#ifndef EVENTIRCUSERSTATUSREQUEST_H
#define EVENTIRCUSERSTATUSREQUEST_H

#include "IIrcCommand.hpp"
#include <list>
#include <string>


class IrcChannelLoginData;
class EventIrcUserStatusRequest : public IIrcCommand {
public:
    enum class Status {
        Join,
        Part
    };
private:
    Status status;
    size_t userId;
    size_t serverId;
    std::string target; /// can be a channel or user
    std::string password; /// in case of joins
public:
    static constexpr UUID uuid = 24;
    virtual UUID getEventUuid() const override;

    explicit EventIrcUserStatusRequest(Status status,
                                       size_t userId,
                                       size_t serverId,
                                       const std::string& target = "",
                                       const std::string& password = "");
    virtual size_t getUserId() const override;
    virtual size_t getServerId() const override;
    Status getStatus() const;
    std::string getTarget() const;
    std::string getPassword() const;
};

#endif
