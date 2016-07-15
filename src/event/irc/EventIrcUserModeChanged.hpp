#ifndef EVENTIRCUSERMODECHANGED_H
#define EVENTIRCUSERMODECHANGED_H

#include "../IClientEvent.hpp"
#include <string>


class EventIrcUserModeChanged : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string username;
    std::string channel;
    std::string mode;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcUserModeChanged(size_t userId, size_t serverId,
                            const std::string& username,
                            const std::string& channel,
                            const std::string& mode);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getUsername() const;
    std::string getChannel() const;
    std::string getUserMode() const;
};

#endif
