#include "GenericIniDatabase.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include "queue/EventQueue.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"
#include "utils/Filesystem.hpp"

using std::cout;
using std::endl;


GenericIniDatabase::GenericIniDatabase(const std::string& type)
    : type_{type}
{ }

GenericIniDatabase::~GenericIniDatabase() = default;


bool GenericIniDatabase::addServer(size_t userId,
                                   const std::string& serverName,
                                   size_t& serverId) {
    std::stringstream serversConfigFilename;
    serversConfigFilename
        << "config/user" << userId
        << "/" << type_ << ".servers.ini";
    Ini serversConfig(serversConfigFilename.str());

    auto& serverEntry = serversConfig.expectCategory(serverName);
    std::string serverIdStr;
    if (!serversConfig.getEntry(serverEntry, "id", serverIdStr)) {
        serverId = IdProvider::getInstance().generateNewId("server");
        serverIdStr = std::to_string(serverId);
        serversConfig.setEntry(serverEntry, "id", serverIdStr);
        return true;
    }
    cout << "IMPL ERROR: SERVER ALREADY EXISTS" << endl;
    return false;
#pragma message "AddServer: handle server already exists case"
}

bool GenericIniDatabase::deleteServer(size_t userId, size_t serverId) {
    if (serverId <= 0) return false;
    std::stringstream serversConfigFilename;
    serversConfigFilename
        << "config/user" << userId
        << "/" << type_ << ".servers.ini";
    Ini serversConfig(serversConfigFilename.str());

    for (auto& categoryPair : serversConfig) {
        size_t serverId;
        std::string serverIdStr;
        serversConfig.getEntry(categoryPair.second, "id", serverIdStr);
        std::istringstream(serverIdStr) >> serverId;
        if (serverId == serverId) {
            serversConfig.deleteCategory(categoryPair.first);
            return true;
            break;
        }
    }
    return false;
#pragma message "DeleteServer: Cleanup directories"
}

bool GenericIniDatabase::modifyNicks(size_t userId, size_t serverId,
                                     const std::string& oldNick,
                                     const std::string& newNick) {
    if (serverId > 0) {
        std::stringstream serversConfigFilename;
        serversConfigFilename
            << "config/user" << userId
            << "/" << type_ << ".servers.ini";
        Ini serversConfig(serversConfigFilename.str());

        // find server by id
        for (auto& categoryPair : serversConfig) {
            auto& server = categoryPair.second;
            size_t serverId;
            std::string serverIdStr;
            serversConfig.getEntry(server, "id", serverIdStr);
            std::istringstream(serverIdStr) >> serverId;
            if (serverId == serverId) {
                std::string nicksStr;
                serversConfig.getEntry(server, "nicks", nicksStr);
                if (oldNick.size() == 0) {
                    std::stringstream newNicksStr;
                    if (nicksStr.size() > 0)
                        newNicksStr << nicksStr << ",";
                    newNicksStr << newNick; // append nick
                    serversConfig.setEntry(server, "nicks", newNicksStr.str());
                } else {
                    bool first = true;
                    std::istringstream oldNicksStream(nicksStr);
                    std::stringstream newNicksStream;
                    std::string nick;
                    while (getline(oldNicksStream, nick, ',')) {
                        if (nick == oldNick)
                            nick = newNick; // change nick to new one
                        if (nick.size() == 0) continue; // delete nick if empty
                        if (first) {
                            first = false;
                        } else {
                            newNicksStream << ",";
                        }
                        newNicksStream << nick;
                    }
                    serversConfig.setEntry(server, "nicks", newNicksStream.str());
                }
                return true;
            }
        }
    }
    return false;
}

bool GenericIniDatabase::addHost(size_t userId, size_t serverId,
                                 const std::string& host,
                                 int port,
                                 const std::string& password,
                                 bool ipV6,
                                 bool ssl) {
    std::stringstream serverPath;
    serverPath
        << "config/user" << userId
        << "/server" << serverId;

    Filesystem::getInstance().createPathRecursive(serverPath.str());

    std::stringstream hostsConfigFilename;
    hostsConfigFilename << serverPath.str() << "/hosts.ini";
    Ini hostsConfig(hostsConfigFilename.str());

    std::stringstream hostKey;
    hostKey << host << ":" << port;

    bool existed = hostsConfig.hasCategory(hostKey.str());
    if (!existed) {
        auto& hostEntry = hostsConfig.expectCategory(hostKey.str());
        hostsConfig.setEntry(hostEntry, "password", password);
        hostsConfig.setEntry(hostEntry, "ipv6", ipV6 ? "y" : "n");
        hostsConfig.setEntry(hostEntry, "ssl", ssl ? "y" : "n");
        return true;
    } else {
        cout << "IMPL ERROR: HOST ALREADY EXISTS" << endl;
    }
    return false;
#pragma message "AddHost: handle host already exists case"
}

bool GenericIniDatabase::deleteHost(size_t userId, size_t serverId,
                                    const std::string& hostName,
                                    int port) {
    if (serverId > 0) {
        std::stringstream hostsConfigFilename;
        hostsConfigFilename
            << "config/user" << userId
            << "/server" << serverId
            << "/hosts.ini";
        Ini hostsConfig(hostsConfigFilename.str());

        std::stringstream hostKey;
        hostKey << hostName << ":" << port;
        hostsConfig.deleteCategory(hostKey.str());
    }
}

bool GenericIniDatabase::joinChannel(size_t userId, size_t serverId,
                                     std::function<void(Ini&)> entryCallback) {
    // construct channels.ini path
    std::stringstream serverChannelsConfigFilename;
    serverChannelsConfigFilename
        << "config/user" << userId
        << "/server" << serverId
        << "/channels.ini";

    Ini channelsConfig(serverChannelsConfigFilename.str());

    if (entryCallback) entryCallback(channelsConfig);
}

bool GenericIniDatabase::partChannel(size_t userId, size_t serverId,
                                     std::function<void(Ini&)> entryCallback) {
    // construct channel.ini path
    std::stringstream serverChannelsConfigFilename;
    serverChannelsConfigFilename
        << "config/user" << userId
        << "/server" << serverId
        << "/channels.ini";

    Ini channelsConfig(serverChannelsConfigFilename.str());

    if (entryCallback) entryCallback(channelsConfig);
}

bool GenericIniDatabase::deleteChannel(size_t userId, size_t serverId,
                                       const std::string& channelName) {

    // construct channel.ini path
    std::stringstream serverChannelsConfigFilename;
    serverChannelsConfigFilename
        << "config/user" << userId
        << "/server" << serverId
        << "/channels.ini";

    Ini channelsConfig(serverChannelsConfigFilename.str());

    // save data from join event to ini
    auto* categoryEntry = channelsConfig.getEntry(channelName);
    if (categoryEntry)
        channelsConfig.deleteCategory(channelName);
}
