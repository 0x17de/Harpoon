#ifndef GENERICINIDATABASE_H
#define GENERICINIDATABASE_H

#include <string>
#include <functional>
#include "utils/Ini.hpp"


/// Base class for settings databases using ini files
class EventQueue;
class GenericIniDatabase {
    std::string type_;

protected:
    GenericIniDatabase(const std::string& type);
    virtual ~GenericIniDatabase();

    /// Adds a server into "config/user${userId}/${type_}.servers.ini"
    bool addServer(size_t userId,
                   const std::string& serverName,
                   size_t& serverId);
    /// Deletes a server from the config
    bool deleteServer(size_t userId, size_t serverId);
    /// Changes the default login nicks for a server
    bool modifyNicks(size_t userId, size_t serverId,
                     const std::string& oldNick,
                     const std::string& newNick);
    /// Adds a host connection to a server into
    /// "config/user${userId}/server${serverId}/hosts.ini"
    bool addHost(size_t userId, size_t serverId,
                 const std::string& hostName,
                 int port,
                 const std::string& password,
                 bool ipv6,
                 bool ssl);
    /// Deletes a host entry in the config
    bool deleteHost(size_t userId, size_t serverId,
                    const std::string& hostName,
                    int port);
    /// Opens the channel entry from the config and passes the ini handle to the callback
    bool joinChannel(size_t userId, size_t serverId,
                     std::function<void(Ini&)> entryCallback);
    /// Opens the channel entry from the config and passes the ini handle to the callback
    bool partChannel(size_t userId, size_t serverId,
                     std::function<void(Ini&)> entryCallback);
    /// Completely erases a channel from the configuration file
    bool deleteChannel(size_t userId, size_t serverId,
                       const std::string& channelName);
};

#endif
