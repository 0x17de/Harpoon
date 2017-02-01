#ifndef EVENTIRCBACKLOGRESPONSE_H
#define EVENTIRCBACKLOGRESPONSE_H

#include "../IClientEvent.hpp"
#include <string>
#include <list>
#include <memory>


class EventIrcBacklogResponse : public IClientEvent {
    size_t userId;
    size_t serverId;
    std::string channel;
    std::list<std::shared_ptr<IEvent>> events;

public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventIrcBacklogResponse(size_t userId,
                            size_t serverId,
                            const std::string& channel,
                            std::list<std::shared_ptr<IEvent>>&& events);

    virtual size_t getUserId() const override;
    size_t getServerId() const;
    std::string getChannel() const;
    const std::list<std::shared_ptr<IEvent>>& getEvents() const;
};

#endif
