#include "IrcDatabase_Ini.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcPartChannel.hpp"
#include "event/irc/EventIrcAddServer.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
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
        EventInit::uuid,
        EventQuit::uuid,
        EventLoginResult::uuid,
        EventIrcAddServer::uuid
    }),
    appQueue{appQueue}
{
}

bool IrcDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "IrcDB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventIrcAddServer::uuid) {
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
#warning EventIrcAddServer: handle server already exists case
        }
    } else if (eventType == EventIrcJoinChannel::uuid) {
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
            auto& channelEntry = channelsConfig.expectCategory(loginData.getChannelName());
            string channelId;
            if (!channelsConfig.getEntry(channelEntry, "id", channelId)) {
                channelId = to_string(IdProvider::getInstance().generateNewId("channel"));
                channelsConfig.setEntry(channelEntry, "id", channelId);
            }
            channelsConfig.setEntry(channelEntry, "password", loginData.getChannelPassword());
        }
    } else if (eventType == EventIrcPartChannel::uuid) {
        auto part = event->as<EventIrcPartChannel>();

        // construct channel.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
           << "config/user" << part->getUserId()
            << "/server" << part->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        for (const auto& channelName : part->getChannels())
            channelsConfig.deleteCategory(channelName); // no need to remember (this is no backlog)
    } else if (eventType == EventInit::uuid) {
        std::cout << "IrcDB received INIT event" << std::endl;

        // activate service per user
        Ini usersConfig("config/users.ini");
        for (auto& userCategoryPair : usersConfig) {
            auto& userCategory = userCategoryPair.second;
            string userName = userCategoryPair.first;

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
                    string channelIdStr, channelPassword;
                    channelsConfig.getEntry(channel, "id", channelIdStr);
                    channelsConfig.getEntry(channel, "password", channelPassword);
                    istringstream(channelIdStr) >> channelId;

                    // add channel to event
                    loginConfiguration.addChannelLoginData(channelId, channelName, channelPassword);
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
    }
    return true;
}
