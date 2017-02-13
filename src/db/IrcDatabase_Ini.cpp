#include "IrcDatabase_Ini.hpp"
#include "event/irc/EventIrcServiceInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcPartChannel.hpp"
#include "event/irc/EventIrcDeleteChannel.hpp"
#include "event/irc/EventIrcAddServer.hpp"
#include "event/irc/EventIrcDeleteServer.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
#include "event/irc/EventIrcServerDeleted.hpp"
#include "event/irc/EventIrcAddHost.hpp"
#include "event/irc/EventIrcHostAdded.hpp"
#include "event/irc/EventIrcDeleteHost.hpp"
#include "event/irc/EventIrcHostDeleted.hpp"
#include "event/irc/EventIrcModifyNick.hpp"
#include "utils/Filesystem.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"
#include "utils/ModuleProvider.hpp"
#include <iostream>
#include <sstream>

using namespace std;


PROVIDE_MODULE("irc_database", "ini", IrcDatabase_Ini);

IrcDatabase_Ini::IrcDatabase_Ini(EventQueue* appQueue) :
    EventLoop({
        EventIrcServiceInit::uuid,
        EventQuit::uuid,
        EventLoginResult::uuid,
        EventIrcAddServer::uuid,
        EventIrcDeleteServer::uuid,
        EventIrcAddHost::uuid,
        EventIrcDeleteHost::uuid,
        EventIrcModifyNick::uuid,
        EventIrcJoinChannel::uuid,
        EventIrcPartChannel::uuid,
        EventIrcDeleteChannel::uuid
            }),
    appQueue{appQueue}
{
}

IrcDatabase_Ini::~IrcDatabase_Ini() {
}

bool IrcDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    switch(eventType) {
    case EventQuit::uuid: {
        std::cout << "IrcDB received QUIT event" << std::endl;
        return false;
    }
    case EventIrcAddServer::uuid: {
        auto add = event->as<EventIrcAddServer>();

        stringstream serversConfigFilename;
        serversConfigFilename
            << "config/user" << add->getUserId()
            << "/irc.servers.ini";
        Ini serversConfig(serversConfigFilename.str());

        auto& serverEntry = serversConfig.expectCategory(add->getName());
        string serverIdStr;
        if (!serversConfig.getEntry(serverEntry, "id", serverIdStr)) {
            size_t serverId = IdProvider::getInstance().generateNewId("server");
            serverIdStr = to_string(serverId);
            serversConfig.setEntry(serverEntry, "id", serverIdStr);

            appQueue->sendEvent(make_shared<EventIrcServerAdded>(add->getUserId(), serverId, add->getName()));
        } else {
            cout << "IMPL ERROR: SERVER ALREADY EXISTS" << endl;
#pragma message "EventIrcAddServer: handle server already exists case"
        }
        break;
    }
    case EventIrcDeleteServer::uuid: {
        auto del = event->as<EventIrcDeleteServer>();
        if (del->getServerId() > 0) {
            stringstream serversConfigFilename;
            serversConfigFilename
                << "config/user" << del->getUserId()
                << "/irc.servers.ini";
            Ini serversConfig(serversConfigFilename.str());

            for (auto& categoryPair : serversConfig) {
                size_t serverId;
                string serverIdStr;
                serversConfig.getEntry(categoryPair.second, "id", serverIdStr);
                istringstream(serverIdStr) >> serverId;
                if (serverId == del->getServerId()) {
                    serversConfig.deleteCategory(categoryPair.first);
                    appQueue->sendEvent(make_shared<EventIrcServerDeleted>(del->getUserId(), del->getServerId()));
                    break;
                }
            }
        }
        break;
#pragma message "EventIrcDeleteServer: Cleanup directories"
    }
    case EventIrcModifyNick::uuid: {
        auto modify = event->as<EventIrcModifyNick>();
        if (modify->getServerId() > 0) {
            stringstream serversConfigFilename;
            serversConfigFilename
                << "config/user" << modify->getUserId()
                << "/irc.servers.ini";
            Ini serversConfig(serversConfigFilename.str());

            // find server by id
            for (auto& categoryPair : serversConfig) {
                auto& server = categoryPair.second;
                size_t serverId;
                string serverIdStr;
                serversConfig.getEntry(server, "id", serverIdStr);
                istringstream(serverIdStr) >> serverId;
                if (serverId == modify->getServerId()) {
                    string nicksStr;
                    serversConfig.getEntry(server, "nicks", nicksStr);
                    if (modify->getOldNick().size() == 0) {
                        stringstream newNicksStr;
                        if (nicksStr.size() > 0)
                            newNicksStr << nicksStr << ",";
                        newNicksStr << modify->getNewNick(); // append nick
                        serversConfig.setEntry(server, "nicks", newNicksStr.str());
                    } else {
                        bool first = true;
                        istringstream oldNicksStream(nicksStr);
                        stringstream newNicksStream;
                        string nick;
                        while (getline(oldNicksStream, nick, ',')) {
                            if (nick == modify->getOldNick())
                                nick = modify->getNewNick(); // change nick to new one
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
                    break; // nicks were changed
                }
            }
        }
        break;
    }
    case EventIrcAddHost::uuid: {
        auto add = event->as<EventIrcAddHost>();

        stringstream serverPath;
        serverPath
            << "config/user" << add->getUserId()
            << "/server" << add->getServerId();

        Filesystem::getInstance().createPathRecursive(serverPath.str());

        stringstream hostsConfigFilename;
        hostsConfigFilename << serverPath.str() << "/hosts.ini";
        Ini hostsConfig(hostsConfigFilename.str());

        stringstream hostKey;
        hostKey << add->getHost() << ":" << add->getPort();

        bool existed = hostsConfig.hasCategory(hostKey.str());
        if (!existed) {
            auto& hostEntry = hostsConfig.expectCategory(hostKey.str());
            hostsConfig.setEntry(hostEntry, "password", add->getPassword());
            hostsConfig.setEntry(hostEntry, "ipv6", add->getIpV6() ? "y" : "n");
            hostsConfig.setEntry(hostEntry, "ssl", add->getSsl() ? "y" : "n");
            appQueue->sendEvent(make_shared<EventIrcHostAdded>(add->getUserId(),
                                                               add->getServerId(),
                                                               add->getHost(),
                                                               add->getPort(),
                                                               add->getPassword(),
                                                               add->getIpV6(),
                                                               add->getSsl()));
        } else {
            cout << "IMPL ERROR: HOST ALREADY EXISTS" << endl;
#pragma message "EventIrcAddHost: handle host already exists case"
        }
        break;
    }
    case EventIrcDeleteHost::uuid: {
        auto del = event->as<EventIrcDeleteHost>();
        if (del->getServerId() > 0) {
            stringstream hostsConfigFilename;
            hostsConfigFilename
                << "config/user" << del->getUserId()
                << "/server" << del->getServerId()
                << "/hosts.ini";
            Ini hostsConfig(hostsConfigFilename.str());

            stringstream hostKey;
            hostKey << del->getHost() << ":" << del->getPort();
            hostsConfig.deleteCategory(hostKey.str());
            appQueue->sendEvent(make_shared<EventIrcHostDeleted>(del->getUserId(), del->getServerId(), del->getHost(), del->getPort()));
        }
        break;
#pragma message "EventIrcDeleteServer: Cleanup directories"
    }
    case EventIrcJoinChannel::uuid: {
        auto join = event->as<EventIrcJoinChannel>();

        // construct channels.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
            << "config/user" << join->getUserId()
            << "/server" << join->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        for (auto& loginData : join->getLoginData()) {
            auto& channelEntry = channelsConfig.expectCategory(loginData.name);
            string channelId;
            if (!channelsConfig.getEntry(channelEntry, "id", channelId)) {
                channelId = to_string(IdProvider::getInstance().generateNewId("channel"));
                channelsConfig.setEntry(channelEntry, "id", channelId);
                channelsConfig.setEntry(channelEntry, "password", loginData.password);
            }
            channelsConfig.setEntry(channelEntry, "disabled", "no");
            if (loginData.passwordSpecified)
                channelsConfig.setEntry(channelEntry, "password", loginData.password);
        }
        break;
    }
    case EventIrcPartChannel::uuid: {
        auto part = event->as<EventIrcPartChannel>();

        // construct channel.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
            << "config/user" << part->getUserId()
            << "/server" << part->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        for (const auto& channelName : part->getChannels()) {
            auto* categoryEntry = channelsConfig.getEntry(channelName);
            if (categoryEntry)
                channelsConfig.setEntry(*categoryEntry, "disabled", "yes");
        }
        break;
    }
    case EventIrcDeleteChannel::uuid: {
        auto deleteCommand = event->as<EventIrcDeleteChannel>();

        // construct channel.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
            << "config/user" << deleteCommand->getUserId()
            << "/server" << deleteCommand->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        string channelName = deleteCommand->getChannelName();
        auto* categoryEntry = channelsConfig.getEntry(channelName);
        if (categoryEntry)
            channelsConfig.deleteCategory(channelName);
        break;
    }
    case EventIrcServiceInit::uuid: {
        std::cout << "IrcDB received INIT event" << std::endl;

        // activate service per user
        Ini usersConfig("config/users.ini");
        for (auto& userCategoryPair : usersConfig) {
            auto& userCategory = userCategoryPair.second;
            //string userName = userCategoryPair.first;

            // read in user configuration
            size_t userId;
            string userIdStr;
            usersConfig.getEntry(userCategory, "id", userIdStr);
            istringstream(userIdStr) >> userId;

            // initialize event
            auto login = make_shared<EventIrcActivateService>(userId);

            // create user folder if it does not exist yet
            stringstream userDirectory;
            userDirectory << "config/user" << userId;
            Filesystem::getInstance().createPathRecursive(userDirectory.str());

            // load server settings
            stringstream serversConfigFilename;
            serversConfigFilename << userDirectory.str() << "/irc.servers.ini";
            Ini serversConfig(serversConfigFilename.str());

            // add server configuration
            for (auto& serverCategoryPair : serversConfig) {
                auto& serverCategory = serverCategoryPair.second;
                string serverName = serverCategoryPair.first;

                // read server settings
                size_t serverId;
                string serverIdStr, nicksStr;
                serversConfig.getEntry(serverCategory, "id", serverIdStr);
                serversConfig.getEntry(serverCategory, "nicks", nicksStr);
                istringstream(serverIdStr) >> serverId;

                // initialize event
                auto& loginConfiguration = login->addLoginConfiguration(serverId, serverName);

                // create settings folder if it does not exist yet
                stringstream serverDirectory;
                serverDirectory << userDirectory.str() << "/server" << serverId;
                Filesystem::getInstance().createPathRecursive(serverDirectory.str());

                // load hosts settings
                stringstream hostsConfigFilename;
                hostsConfigFilename << serverDirectory.str() << "/hosts.ini";
                Ini hostsConfig(hostsConfigFilename.str());
                // add host configuration
                for (auto& hostCategoryPair : hostsConfig) {
                    auto& host = hostCategoryPair.second;
                    string hostData = hostCategoryPair.first;

                    // construct host config from name
                    string hostName;
                    int port;
                    istringstream hostDataIs(hostData);
                    getline(hostDataIs, hostName, ':');
                    hostDataIs >> port;

                    // read host settings
                    string password, ipV6, ssl;
                    hostsConfig.getEntry(host, "password", password);
                    hostsConfig.getEntry(host, "ipv6", ipV6);
                    hostsConfig.getEntry(host, "ssl", ssl);

                    loginConfiguration.addHostConfiguration(hostName,
                                                            port,
                                                            password,
                                                            ipV6 == "y",
                                                            ssl == "y");
                }

                // load channel configuration
                stringstream serverChannelsConfigFilename;
                serverChannelsConfigFilename << serverDirectory.str() << "/channels.ini";
                Ini channelsConfig(serverChannelsConfigFilename.str());

                // add channel configuration
                for (auto& channelCategoryPair : channelsConfig) {
                    auto& channel = channelCategoryPair.second;
                    string channelName = channelCategoryPair.first;

                    // read channel settings
                    size_t channelId;
                    string channelIdStr, channelPassword, channelDisabled;
                    channelsConfig.getEntry(channel, "id", channelIdStr);
                    channelsConfig.getEntry(channel, "password", channelPassword);
                    if (!channelsConfig.getEntry(channel, "disabled", channelDisabled))
                        channelDisabled = "no";
                    istringstream(channelIdStr) >> channelId;

                    // add channel to event
                    loginConfiguration.addChannelLoginData(channelId, channelName, channelPassword, channelDisabled == "yes");
                }

                // add nicks
                istringstream nickStream(nicksStr);
                string nick;
                while (getline(nickStream, nick, ','))
                    loginConfiguration.addNick(nick);
            }

            // dispatch event
            appQueue->sendEvent(login);
        }
        break;
    }
    } // switch(eventType)
    return true;
}
