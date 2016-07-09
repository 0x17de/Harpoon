#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include "ISingleClientEvent.hpp"
#include <memory>


class EventQueue;
class EventLoginResult : public ISingleClientEvent {
    bool success;
    size_t userId;
    void* data;
public:
    static UUID uuid;
    virtual UUID getEventUuid() const override;

    EventLoginResult(bool success, size_t userId, void* data);

    virtual size_t getUserId() const override;
    virtual void* getData() const override;
    bool getSuccess() const;
};

#endif
