#ifndef EVENTLOGIN_H
#define EVENTLOGIN_H

#include <memory>
#include "IEvent.hpp"


class EventQueue;
/// Event for checking if the user's credentials are correct
class EventLogin : public IEvent {
    void* data;
    std::string username;
    std::string password;
public:
    /// Constructor
    ///
    /// \param data Custom data for identification purposes
    /// \param username Login username
    /// \param password Login password
    EventLogin(void* data, const std::string& username, const std::string& password);

    static constexpr UUID uuid = 4;
    virtual UUID getEventUuid() const override;

    EventQueue* getTarget() const;
    void* getData() const;
    std::string getUsername() const;
    std::string getPassword() const;
};

#endif
