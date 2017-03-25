#include "HackDatabase_Ini.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/hack/EventHackServiceInit.hpp"
#include "event/hack/EventHackAddServer.hpp"
#include "event/hack/EventHackServerAdded.hpp"
#include "event/hack/EventHackDeleteServer.hpp"
#include "event/hack/EventHackServerDeleted.hpp"
#include "event/hack/EventHackModifyNick.hpp"
#include "event/hack/EventHackAddHost.hpp"
#include "event/hack/EventHackHostAdded.hpp"
#include "event/hack/EventHackDeleteHost.hpp"
#include "event/hack/EventHackHostDeleted.hpp"
#include "event/hack/EventHackJoinChannel.hpp"
#include "event/hack/EventHackPartChannel.hpp"
#include "event/hack/EventHackDeleteChannel.hpp"
#include "event/hack/EventHackActivateService.hpp"
#include "service/hack/HackServerConfiguration.hpp"
#include "utils/Filesystem.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"
#include "utils/ModuleProvider.hpp"
#include <iostream>
#include <sstream>

using namespace std;


PROVIDE_EVENTLOOP_MODULE("hack_database", "ini", HackDatabase_Ini)

HackDatabase_Ini::HackDatabase_Ini(EventQueue* appQueue) :
    EventLoop({
        EventHackServiceInit::uuid,
        EventQuit::uuid,
        EventLoginResult::uuid,
    }),
    appQueue{appQueue}
{
}

HackDatabase_Ini::~HackDatabase_Ini() {
}

bool HackDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    switch(eventType) {
    case EventQuit::uuid: {
        std::cout << "HackDB received QUIT event" << std::endl;
        return false;
    }
    case EventHackAddServer::uuid: {
        auto add = event->as<EventHackAddServer>();

        stringstream serversConfigFilename;
        serversConfigFilename
            << "config/user" << add->getUserId()
            << "/hack.servers.ini";
        Ini serversConfig(serversConfigFilename.str());

        auto& serverEntry = serversConfig.expectCategory(add->getName());
        string serverIdStr;
        if (!serversConfig.getEntry(serverEntry, "id", serverIdStr)) {
            size_t serverId = IdProvider::getInstance().generateNewId("server");
            serverIdStr = to_string(serverId);
            serversConfig.setEntry(serverEntry, "id", serverIdStr);

            appQueue->sendEvent(make_shared<EventHackServerAdded>(add->getUserId(), serverId, add->getName()));
        } else {
            cout << "IMPL ERROR: SERVER ALREADY EXISTS" << endl;
        }
        break;
    }
    case EventHackDeleteServer::uuid: {
        auto del = event->as<EventHackDeleteServer>();
        if (del->getServerId() > 0) {
            stringstream serversConfigFilename;
            serversConfigFilename
                << "config/user" << del->getUserId()
                << "/hack.servers.ini";
            Ini serversConfig(serversConfigFilename.str());

            for (auto& categoryPair : serversConfig) {
                size_t serverId;
                string serverIdStr;
                serversConfig.getEntry(categoryPair.second, "id", serverIdStr);
                istringstream(serverIdStr) >> serverId;
                if (serverId == del->getServerId()) {
                    serversConfig.deleteCategory(categoryPair.first);
                    appQueue->sendEvent(make_shared<EventHackServerDeleted>(del->getUserId(), del->getServerId()));
                    break;
                }
            }
        }
        break;
    }
    case EventHackModifyNick::uuid: {
        auto modify = event->as<EventHackModifyNick>();
        if (modify->getServerId() > 0) {
            stringstream serversConfigFilename;
            serversConfigFilename
                << "config/user" << modify->getUserId()
                << "/hack.servers.ini";
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
    case EventHackAddHost::uuid: {
        auto add = event->as<EventHackAddHost>();

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
            hostsConfig.setEntry(hostEntry, "websocketUri", add->getWebsocketUri());
            hostsConfig.setEntry(hostEntry, "ipv6", add->getIpV6() ? "y" : "n");
            hostsConfig.setEntry(hostEntry, "ssl", add->getSsl() ? "y" : "n");
            appQueue->sendEvent(make_shared<EventHackHostAdded>(add->getUserId(),
                                                                add->getServerId(),
                                                                add->getHost(),
                                                                add->getWebsocketUri(),
                                                                add->getPort(),
                                                                add->getPassword(),
                                                                add->getIpV6(),
                                                                add->getSsl()));
        } else {
            cout << "IMPL ERROR: HOST ALREADY EXISTS" << endl;
        }
        break;
    }
    case EventHackDeleteHost::uuid: {
        auto del = event->as<EventHackDeleteHost>();
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
            appQueue->sendEvent(make_shared<EventHackHostDeleted>(del->getUserId(), del->getServerId(), del->getHost(), del->getPort()));
        }
        break;
    }
    case EventHackJoinChannel::uuid: {
        auto join = event->as<EventHackJoinChannel>();

        // construct channels.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
            << "config/user" << join->getUserId()
            << "/server" << join->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        auto& channelEntry = channelsConfig.expectCategory(join->getChannel());
        string channelId;
        if (!channelsConfig.getEntry(channelEntry, "id", channelId)) {
            channelId = to_string(IdProvider::getInstance().generateNewId("channel"));
            channelsConfig.setEntry(channelEntry, "id", channelId);
        }
        channelsConfig.setEntry(channelEntry, "disabled", "no");
        break;
    }
    case EventHackPartChannel::uuid: {
        auto part = event->as<EventHackPartChannel>();

        // construct channel.ini path
        stringstream serverChannelsConfigFilename;
        serverChannelsConfigFilename
            << "config/user" << part->getUserId()
            << "/server" << part->getServerId()
            << "/channels.ini";

        Ini channelsConfig(serverChannelsConfigFilename.str());

        // save data from join event to ini
        auto* categoryEntry = channelsConfig.getEntry(part->getChannel());
        if (categoryEntry)
            channelsConfig.setEntry(*categoryEntry, "disabled", "yes");
        break;
    }
    case EventHackDeleteChannel::uuid: {
        auto deleteCommand = event->as<EventHackDeleteChannel>();

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
    case EventHackServiceInit::uuid: {
        std::cout << "HackDB received INIT event" << std::endl;

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
            auto login = make_shared<EventHackActivateService>(userId);

            // create user folder if it does not exist yet
            stringstream userDirectory;
            userDirectory << "config/user" << userId;
            Filesystem::getInstance().createPathRecursive(userDirectory.str());

            // load server settings
            stringstream serversConfigFilename;
            serversConfigFilename << userDirectory.str() << "/hack.servers.ini";
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
                    string password;
                    int port;
                    istringstream hostDataIs(hostData);
                    getline(hostDataIs, hostName, ':');
                    hostDataIs >> port;

                    // read host settings
                    string websocketUri, ipV6, ssl;
                    hostsConfig.getEntry(host, "websocketUri", websocketUri);
                    hostsConfig.getEntry(host, "password", password);
                    hostsConfig.getEntry(host, "ipv6", ipV6);
                    hostsConfig.getEntry(host, "ssl", ssl);

                    loginConfiguration.addHostConfiguration(hostName,
                                                            websocketUri,
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
                    if (!channelsConfig.getEntry(channel, "disabled", channelDisabled))
                        channelDisabled = "no";
                    istringstream(channelIdStr) >> channelId;

                    // add channel to event
                    loginConfiguration.addChannelLoginData(channelId, channelName, channelDisabled == "yes");
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
