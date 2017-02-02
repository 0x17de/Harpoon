#ifndef EVENTIRCADDSERVER_H
#define EVENTIRCADDSERVER_H

#include "../IUserEvent.hpp"
#include <string>


class EventIrcAddServer : public IUserEvent {
    size_t userId;
    std::string name;
public:
    static constexpr UUID uuid = 13;
    virtual UUID getEventUuid() const override;

    EventIrcAddServer(size_t userId,
                      const std::string& name);
    virtual size_t getUserId() const override;
    std::string getName() const;
};

#endif
