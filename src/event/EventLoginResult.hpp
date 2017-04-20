#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include "ISingleClientEvent.hpp"
#include <memory>


class EventQueue;
/// Response which is sent from the login database on successful login
class EventLoginResult : public ISingleClientEvent {
    bool success;
    size_t userId;
    void* data;
public:
    static constexpr UUID uuid = 5;
    virtual UUID getEventUuid() const override;

    EventLoginResult(bool success, size_t userId, void* data);

    virtual size_t getUserId() const override;
    virtual void* getData() const override;
    bool getSuccess() const;
};

#endif
