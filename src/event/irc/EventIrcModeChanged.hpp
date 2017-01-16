#ifndef EVENTIRCMODECHANGED_H
#define EVENTIRCMODECHANGED_H

#include "../IClientEvent.hpp"
#include "IrcLoggable.hpp"
#include <string>
#include <vector>


class EventIrcModeChanged : public IClientEvent, public IrcLoggable {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    std::string mode;
    std::vector<std::string> args;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcModeChanged(size_t userId, size_t serverId,
                        const std::string& username,
                        const std::string& channel,
                        const std::string& mode,
                        std::vector<std::string>::const_iterator argsBegin,
                        std::vector<std::string>::const_iterator argsEnd);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    std::string getMode() const;
    const std::vector<std::string>& getArgs() const;
};

#endif
