#include "EventLogin.hpp"
#include "utils/uuidGen.hpp"


UUID EventLogin::uuid = ::uuid.get();
UUID EventLogin::getEventUuid() const {
    return this->uuid;
}

EventLogin::EventLogin(void* data, std::string username, std::string password)
    : data{data}
    , username{username}
    , password{password}
{
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
