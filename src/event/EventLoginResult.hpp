#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include <memory>
#include "IEvent.hpp"


class EventQueue;
class EventLoginResult : public IEvent {
    EventQueue* target;
    bool success;
    size_t userId;
    void* data;
public:
    EventLoginResult(EventQueue* target, bool success, size_t userId, void* data);

    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventQueue* getTarget() const;
    bool getSuccess() const;
    size_t getUserId() const;
    void* getData() const;
};

#endif
