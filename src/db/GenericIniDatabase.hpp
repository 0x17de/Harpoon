#ifndef GENERICINIDATABASE_H
#define GENERICINIDATABASE_H

#include <string>
#include <functional>
#include "utils/Ini.hpp"


class EventQueue;
class GenericIniDatabase {
    std::string type_;

protected:
    GenericIniDatabase(const std::string& type);
    virtual ~GenericIniDatabase();

    bool addServer(size_t userId,
                   const std::string& serverName,
                   size_t& serverId);
    bool deleteServer(size_t userId, size_t serverId);
    bool modifyNicks(size_t userId, size_t serverId,
                     const std::string& oldNick,
                     const std::string& newNick);
    bool addHost(size_t userId, size_t serverId,
                 const std::string& hostName,
                 int port,
                 const std::string& password,
                 bool ipv6,
                 bool ssl);
    bool deleteHost(size_t userId, size_t serverId,
                    const std::string& hostName,
                    int port);
    bool joinChannel(size_t userId, size_t serverId,
                     std::function<void(Ini&)> entryCallback);
    bool partChannel(size_t userId, size_t serverId,
                     std::function<void(Ini&)> entryCallback);
    bool deleteChannel(size_t userId, size_t serverId,
                       const std::string& channelName);
};

#endif
