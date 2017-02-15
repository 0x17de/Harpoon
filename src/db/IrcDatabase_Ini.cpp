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
    GenericIniDatabase("irc"),
    appQueue_{appQueue}
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
        size_t userId = add->getUserId();
        std::string serverName = add->getName();
        size_t serverId; // returned via add function on success

        if (addServer(userId,
                      serverName,
                      serverId)) {
            appQueue_->sendEvent(std::make_shared<EventIrcServerAdded>(userId, serverId, serverName));
        }
        break;
    }
    case EventIrcDeleteServer::uuid: {
        auto del = event->as<EventIrcDeleteServer>();
        size_t userId = del->getUserId();
        size_t serverId = del->getServerId();

        if (deleteServer(userId,
                         serverId)) {
            appQueue_->sendEvent(std::make_shared<EventIrcServerDeleted>(userId, serverId));
        }
        break;
    }
    case EventIrcModifyNick::uuid: {
        auto modify = event->as<EventIrcModifyNick>();
        size_t userId = modify->getUserId();
        size_t serverId = modify->getServerId();
        std::string oldNick = modify->getOldNick();
        std::string newNick = modify->getNewNick();

        if (modifyNicks(userId, serverId,
                        oldNick, newNick)) {
            // no handling for success
        }
        break;
    }
    case EventIrcAddHost::uuid: {
        auto add = event->as<EventIrcAddHost>();
        size_t userId = add->getUserId();
        size_t serverId = add->getServerId();
        std::string hostName = add->getHost();
        int port = add->getPort();
        std::string password = add->getPassword();
        bool ipV6 = add->getIpV6();
        bool ssl = add->getSsl();

        if (addHost(userId, serverId,
                    hostName, port,
                    password,
                    ipV6, ssl)) {
            appQueue_->sendEvent(make_shared<EventIrcHostAdded>(userId,
                                                                serverId,
                                                                hostName,
                                                                port,
                                                                password,
                                                                ipV6,
                                                                ssl));
        }
        break;
    }
    case EventIrcDeleteHost::uuid: {
        auto del = event->as<EventIrcDeleteHost>();
        size_t userId = del->getUserId();
        size_t serverId = del->getServerId();
        std::string hostName = del->getHost();
        int port = del->getPort();

        if (deleteHost(userId, serverId, hostName, port)) {
            appQueue_->sendEvent(std::make_shared<EventIrcHostDeleted>(userId, serverId, hostName, port));
        }
        break;
#pragma message "EventIrcDeleteServer: Cleanup directories"
    }
    case EventIrcJoinChannel::uuid: {
        auto join = event->as<EventIrcJoinChannel>();
        size_t userId = join->getUserId();
        size_t serverId = join->getServerId();

        if (joinChannel(userId, serverId,
                        [&join](Ini& ini){
                            // save whatever you need here
                            // save data from join event to ini
                            for (auto& loginData : join->getLoginData()) {
                                auto& channelEntry = ini.expectCategory(loginData.name);
                                string channelId;
                                if (!ini.getEntry(channelEntry, "id", channelId)) {
                                    channelId = to_string(IdProvider::getInstance().generateNewId("channel"));
                                    ini.setEntry(channelEntry, "id", channelId);
                                    ini.setEntry(channelEntry, "password", loginData.password);
                                }
                                ini.setEntry(channelEntry, "disabled", "no");
                                if (loginData.passwordSpecified) {
                                    ini.setEntry(channelEntry, "password", loginData.password);
                                }
                            }
                        })) {
            // no action
        }

        break;
    }
    case EventIrcPartChannel::uuid: {
        auto part = event->as<EventIrcPartChannel>();
        size_t userId = part->getUserId();
        size_t serverId = part->getServerId();

        if (partChannel(userId, serverId,
                        [&part](Ini& ini){
                            // save data from part event to ini
                            for (const auto& channelName : part->getChannels()) {
                                auto* categoryEntry = ini.getEntry(channelName);
                                if (categoryEntry)
                                    ini.setEntry(*categoryEntry, "disabled", "yes");
                            }
                        })) {
            // no action
        }
        break;
    }
    case EventIrcDeleteChannel::uuid: {
        auto deleteCommand = event->as<EventIrcDeleteChannel>();
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
                    hostsConfig.getEntry(host, "ipV6", ipV6);
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
            appQueue_->sendEvent(login);
        }
        break;
    }
    } // switch(eventType)
    return true;
}
