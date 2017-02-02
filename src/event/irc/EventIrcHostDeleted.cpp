#include "EventIrcHostDeleted.hpp"


UUID EventIrcHostDeleted::getEventUuid() const {
    return this->uuid;
}

EventIrcHostDeleted::EventIrcHostDeleted(size_t userId,
                                         size_t serverId,
                                         const std::string& host,
                                         int port)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , port{port}
{
}

size_t EventIrcHostDeleted::getUserId() const {
    return userId;
}

size_t EventIrcHostDeleted::getServerId() const {
    return serverId;
}

std::string EventIrcHostDeleted::getHost() const {
    return host;
}

int EventIrcHostDeleted::getPort() const {
    return port;
}
