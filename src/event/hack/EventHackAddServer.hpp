#ifndef EVENTHACKADDSERVER_H
#define EVENTHACKADDSERVER_H

#include "../IUserEvent.hpp"
#include <string>


class EventHackAddServer : public IUserEvent {
    size_t userId;
    std::string name;
public:
    static constexpr UUID uuid = 54;
    virtual UUID getEventUuid() const override;

    EventHackAddServer(size_t userId,
                       const std::string& name);
    virtual size_t getUserId() const override;
    std::string getName() const;
};

#endif
