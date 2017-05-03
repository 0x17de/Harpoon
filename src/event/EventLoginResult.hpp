#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include "ISingleClientEvent.hpp"
#include <memory>


class EventQueue;
/// Response which is sent from the login database on successful login or failure
class EventLoginResult : public ISingleClientEvent {
    bool success;
    size_t userId;
    void* data;
public:
    static constexpr UUID uuid = 5;
    virtual UUID getEventUuid() const override;

    /// Constructor
    ///
    /// \param success True if the login was successful
    /// \param userId UserID of the user for the event
    /// \param data custom data which was provided from a EventLogin
    EventLoginResult(bool success, size_t userId, void* data);

    /// Returns the user id for the specified credentials
    virtual size_t getUserId() const override;
    /// Some user defined data that was sent on the login request event
    virtual void* getData() const override;
    /// Returns if the login credentials were correct
    bool getSuccess() const;
};

#endif
