#include "EventLogin.hpp"
#include "utils/uuidGen.hpp"


UUID EventLogin::uuid = ::uuid.get();
UUID EventLogin::getEventUuid() const {
    return this->uuid;
}

EventLogin::EventLogin(EventQueue* target, void* data, std::string username, std::string password)
    : target{target}
    , data{data}
    , username{username}
    , password{password}
{
}

EventQueue* EventLogin::getTarget() const {
    return target;
}

void* EventLogin::getData() const {
    return data;
}

std::string EventLogin::getUsername() const {
    return username;
}

std::string EventLogin::getPassword() const {
    return password;
}
