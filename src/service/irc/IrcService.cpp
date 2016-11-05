#include "IrcService.hpp"
#include "IrcChannelLoginData.hpp"
#include "IrcServerHostConfiguration.hpp"
#include "IrcServerConfiguration.hpp"
#include "IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/EventQuery.hpp"
#include "event/irc/IrcChannelListing.hpp"
#include "event/irc/IrcServerListing.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "event/irc/EventIrcChatListing.hpp"
#include "event/irc/EventIrcSettingsListing.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
#include "event/irc/EventIrcServerDeleted.hpp"
#include "event/irc/EventIrcHostAdded.hpp"
#include "event/irc/EventIrcHostDeleted.hpp"
#include "event/irc/EventIrcReconnectServer.hpp"
#include "service/irc/IrcChannelStore.hpp"
#include "utils/IdProvider.hpp"
#include <iostream>
#include <mutex>

using namespace std;


IrcService::IrcService(size_t userId, EventQueue* appQueue)
    : EventLoop({
          EventQuit::uuid,
          EventQuery::uuid,
          EventIrcServerAdded::uuid,
          EventIrcServerDeleted::uuid,
          EventIrcHostAdded::uuid,
          EventIrcHostDeleted::uuid,
      }, {
          &EventGuard<IActivateServiceEvent>,
          &EventGuard<IIrcCommand>
      })
    , userId{userId}
    , appQueue{appQueue}
{
}

IrcService::~IrcService() {
}

bool IrcService::onEvent(std::shared_ptr<IEvent> event) {
    UUID type = event->getEventUuid();

    auto activateUser = event->as<EventIrcActivateService>();
    if (activateUser) {
        cout << "[US] Received ActivateUser" << endl;
        auto& loginConfiguration = activateUser->getLoginConfiguration();
        for (auto entry : loginConfiguration) {
            auto& ircConfiguration = entry.second;
            cout << "[US] CONFIG: " << ircConfiguration.getServerId() << endl;
            ircConnections.emplace(piecewise_construct,
                                   forward_as_tuple(ircConfiguration.getServerId()),
                                   forward_as_tuple(appQueue, userId, ircConfiguration));
        }
    }

    if (type == EventQuit::uuid) {
        cout << "[US] received QUIT" << endl;
        ircConnections.clear();
        return false;
    } else if (type == EventIrcReconnectServer::uuid) {
        auto reconnect = event->as<EventIrcReconnectServer>();
        auto it = ircConnections.find(reconnect->getServerId());
        if (it != ircConnections.end()) {
            auto& connection = it->second;
            IrcServerConfiguration configuration = connection.getServerConfiguration();
            ircConnections.erase(it);
            // overwrite old irc connection
            ircConnections.emplace(piecewise_construct,
                                   forward_as_tuple(reconnect->getServerId()),
                                   forward_as_tuple(appQueue,
                                                    reconnect->getUserId(),
                                                    configuration));
        }
    } else if (type == EventIrcServerAdded::uuid) {
        auto add = event->as<EventIrcServerAdded>();
        ircConnections.emplace(piecewise_construct,
                               forward_as_tuple(add->getServerId()),
                               forward_as_tuple(appQueue,
                                                add->getUserId(),
                                                IrcServerConfiguration{add->getServerId(), add->getServerName()}));
    } else if (type == EventIrcServerDeleted::uuid) {
        auto del = event->as<EventIrcServerDeleted>();
        auto it = ircConnections.find(del->getServerId());
        if (it != ircConnections.end()) {
            ircConnections.erase(it);
        }
    } else if (type == EventIrcHostAdded::uuid) {
        auto add = event->as<EventIrcHostAdded>();
        auto it = ircConnections.find(add->getServerId());
        if (it != ircConnections.end()) {
            auto& connection = it->second;
            lock_guard<mutex> lock(connection.getChannelLoginDataMutex());
            connection.addHost(add->getHost(),
                               add->getPort(),
                               add->getPassword(),
                               add->getIpV6(),
                               add->getSsl());
        }
    } else if (type == EventIrcHostDeleted::uuid) {
        auto del = event->as<EventIrcHostDeleted>();
        auto it = ircConnections.find(del->getServerId());
        if (it != ircConnections.end()) {
            auto& connection = it->second;
            lock_guard<mutex> lock(connection.getChannelLoginDataMutex());
            connection.removeHost(del->getHost(), del->getPort());
        }
    } else if (type == EventQuery::uuid) {
        auto query = event->as<EventQuery>();
        if (query->getType() == EventQueryType::Chats) {
            size_t firstId = IdProvider::getInstance().getLastId("irc_log"); // for backlog requests
            auto listing = make_shared<EventIrcChatListing>(firstId, userId, query->getData());

            std::list<lock_guard<mutex>> locks;
            // lock all to assure correct results
            for (auto& cxnPair : ircConnections) {
                auto& connection = cxnPair.second;
                locks.emplace_back(connection.getChannelLoginDataMutex());
            }

            for (auto& cxnPair : ircConnections) {
                auto& connection = cxnPair.second;
                IrcServerListing& server = listing->addServer(connection.getActiveNick(),
                                                            connection.getServerId(),
                                                            connection.getServerName());
                for (auto& channelPair : connection.getChannelStore()) {
                    string channelName = channelPair.first;
                    const IrcChannelStore& channelStore = channelPair.second;
                    IrcChannelListing& channel = server.addChannel(channelName,
                                                                channelStore.getTopic(),
                                                                channelStore.getDisabled());
                    for (auto& userPair : channelStore.getUsers()) {
                        string username = userPair.second.getNick();
                        channel.addUser(username, "");
                    }
                }
            }

            cout << "[US] Sending chat listing" << endl;
            appQueue->sendEvent(listing);
        }
        else if (query->getType() == EventQueryType::Settings) {
            auto listing = make_shared<EventIrcSettingsListing>(userId, query->getData());

            std::list<lock_guard<mutex>> locks;
            // lock all to assure correct results
            for (auto& cxnPair : ircConnections) {
                auto& connection = cxnPair.second;
                locks.emplace_back(connection.getChannelLoginDataMutex());
            }

            for (auto& cxnPair : ircConnections) {
                auto& connection = cxnPair.second;
                auto& serverConfiguration = listing->addServer(connection.getServerId(),
                                                            connection.getServerName());
                auto& connectionServerConfiguration = connection.getServerConfiguration();
                for (auto& nick : connectionServerConfiguration.getNicks())
                    serverConfiguration.addNick(nick);
                for (auto& hostConfiguration : connectionServerConfiguration.getHostConfigurations()) {
                    serverConfiguration.addHostConfiguration(hostConfiguration.getHostName(),
                                                            hostConfiguration.getPort(),
                                                            hostConfiguration.getPassword(),
                                                            hostConfiguration.getIpV6(),
                                                            hostConfiguration.getSsl());
                }
            }

            appQueue->sendEvent(listing);
        }
    } else {
        auto ircCommand = event->as<IIrcCommand>();
        if (ircCommand) {
            auto it = ircConnections.find(ircCommand->getServerId());
            if (it != ircConnections.end())
                it->second.getEventQueue()->sendEvent(event);
        }
    }
    return true;
}
