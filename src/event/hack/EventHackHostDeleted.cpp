#include "EventHackHostDeleted.hpp"


UUID EventHackHostDeleted::getEventUuid() const {
    return this->uuid;
}

EventHackHostDeleted::EventHackHostDeleted(size_t userId,
                                           size_t serverId,
                                           const std::string& host,
                                           int port)
    : userId{userId}
    , serverId{serverId}
    , host{host}
    , port{port}
{
}

size_t EventHackHostDeleted::getUserId() const {
    return userId;
}

size_t EventHackHostDeleted::getServerId() const {
    return serverId;
}

std::string EventHackHostDeleted::getHost() const {
    return host;
}

int EventHackHostDeleted::getPort() const {
    return port;
}
