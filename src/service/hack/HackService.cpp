#include "HackService.hpp"
#include "HackChannelLoginData.hpp"
#include "HackServerHostConfiguration.hpp"
#include "HackServerConfiguration.hpp"
#include "HackConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/EventQuery.hpp"
#include "event/hack/HackChannelListing.hpp"
#include "event/hack/HackServerListing.hpp"
#include "event/hack/EventHackActivateService.hpp"
#include "event/hack/EventHackChatListing.hpp"
#include "event/hack/EventHackSettingsListing.hpp"
#include "event/hack/EventHackServerAdded.hpp"
#include "event/hack/EventHackServerDeleted.hpp"
#include "event/hack/EventHackHostAdded.hpp"
#include "event/hack/EventHackHostDeleted.hpp"
#include "event/hack/EventHackReconnectServer.hpp"
#include "service/hack/HackChannelStore.hpp"
#include "utils/IdProvider.hpp"
#include <iostream>
#include <mutex>

using namespace std;


HackService::HackService(size_t userId, EventQueue* appQueue)
    : EventLoop({
          EventQuit::uuid,
          EventQuery::uuid,
          EventHackServerAdded::uuid,
          EventHackServerDeleted::uuid,
          EventHackHostAdded::uuid,
          EventHackHostDeleted::uuid,
      }, {
          &EventGuard<IActivateServiceEvent>,
          &EventGuard<IHackCommand>
      })
    , userId{userId}
    , appQueue{appQueue}
{
}

HackService::~HackService() {
}

bool HackService::onEvent(std::shared_ptr<IEvent> event) {
    UUID type = event->getEventUuid();

    auto activateUser = event->as<EventHackActivateService>();
    if (activateUser) {
        cout << "[US] Received ActivateUser" << endl;
        auto& loginConfiguration = activateUser->getLoginConfiguration();
        for (auto entry : loginConfiguration) {
            auto& hackConfiguration = entry.second;
            cout << "[US] CONFIG: " << hackConfiguration.getServerId() << endl;
            hackConnections.emplace(piecewise_construct,
                                    forward_as_tuple(hackConfiguration.getServerId()),
                                    forward_as_tuple(appQueue, userId, hackConfiguration));
        }
    }

    if (type == EventQuit::uuid) {
        cout << "[US] received QUIT" << endl;
        hackConnections.clear();
        return false;
    } else if (type == EventHackReconnectServer::uuid) {
        auto reconnect = event->as<EventHackReconnectServer>();
        auto it = hackConnections.find(reconnect->getServerId());
        if (it != hackConnections.end()) {
            auto& connection = it->second;
            HackServerConfiguration configuration = connection.getServerConfiguration();
            hackConnections.erase(it);
            // overwrite old hack connection
            hackConnections.emplace(piecewise_construct,
                                   forward_as_tuple(reconnect->getServerId()),
                                   forward_as_tuple(appQueue,
                                                    reconnect->getUserId(),
                                                    configuration));
        }
    } else if (type == EventHackServerAdded::uuid) {
        auto add = event->as<EventHackServerAdded>();
        hackConnections.emplace(piecewise_construct,
                               forward_as_tuple(add->getServerId()),
                               forward_as_tuple(appQueue,
                                                add->getUserId(),
                                                HackServerConfiguration{add->getServerId(), add->getServerName()}));
    } else if (type == EventHackServerDeleted::uuid) {
        auto del = event->as<EventHackServerDeleted>();
        auto it = hackConnections.find(del->getServerId());
        if (it != hackConnections.end()) {
            hackConnections.erase(it);
        }
    } else if (type == EventHackHostAdded::uuid) {
        auto add = event->as<EventHackHostAdded>();
        auto it = hackConnections.find(add->getServerId());
        if (it != hackConnections.end()) {
            auto& connection = it->second;
            lock_guard<mutex> lock(connection.getChannelLoginDataMutex());
            connection.addHost(add->getHost(),
                               add->getWebsocketUri(),
                               add->getPort(),
                               add->getIpV6(),
                               add->getSsl());
        }
    } else if (type == EventHackHostDeleted::uuid) {
        auto del = event->as<EventHackHostDeleted>();
        auto it = hackConnections.find(del->getServerId());
        if (it != hackConnections.end()) {
            auto& connection = it->second;
            lock_guard<mutex> lock(connection.getChannelLoginDataMutex());
            connection.removeHost(del->getHost(), del->getPort());
        }
    } else if (type == EventQuery::uuid) {
        auto query = event->as<EventQuery>();
        if (query->getType() == EventQueryType::Chats) {
            size_t firstId = IdProvider::getInstance().getLastId("hack_log"); // for backlog requests
            auto listing = make_shared<EventHackChatListing>(firstId, userId, query->getData());

            std::list<lock_guard<mutex>> locks;
            // lock all to assure correct results
            for (auto& cxnPair : hackConnections) {
                auto& connection = cxnPair.second;
                locks.emplace_back(connection.getChannelLoginDataMutex());
            }

            for (auto& cxnPair : hackConnections) {
                auto& connection = cxnPair.second;
                HackServerListing& server = listing->addServer(connection.getActiveNick(),
                                                               connection.getServerId(),
                                                               connection.getServerName());
                for (auto& channelPair : connection.getChannelStore()) {
                    string channelName = channelPair.first;
                    const HackChannelStore& channelStore = channelPair.second;
                    HackChannelListing& channel = server.addChannel(channelName,
                                                                    channelStore.getDisabled());
                    for (auto& userPair : channelStore.getUsers()) {
                        auto& userData = userPair.second;
                        channel.addUser(userData.getNick(), userData.getTrip());
                    }
                }
            }

            cout << "[US] Sending chat listing" << endl;
            appQueue->sendEvent(listing);
        }
        else if (query->getType() == EventQueryType::Settings) {
            /*
            auto listing = make_shared<EventHackSettingsListing>(userId, query->getData());

            std::list<lock_guard<mutex>> locks;
            // lock all to assure correct results
            for (auto& cxnPair : hackConnections) {
                auto& connection = cxnPair.second;
                locks.emplace_back(connection.getChannelLoginDataMutex());
            }

            for (auto& cxnPair : hackConnections) {
                auto& connection = cxnPair.second;
                auto& serverConfiguration = listing->addServer(connection.getServerId(),
                                                               connection.getServerName());
                auto& connectionServerConfiguration = connection.getServerConfiguration();
                for (auto& nick : connectionServerConfiguration.getNicks())
                    serverConfiguration.addNick(nick);
                for (auto& hostConfiguration : connectionServerConfiguration.getHostConfigurations()) {
                    serverConfiguration.addHostConfiguration(hostConfiguration.getHostName(),
                                                            hostConfiguration.getWebsocketUri(),
                                                            hostConfiguration.getPort(),
                                                            hostConfiguration.getIpV6(),
                                                            hostConfiguration.getSsl());
                }
            }

            appQueue->sendEvent(listing);
            */
#pragma message "EventHackSettingsListing: not yet implemented"
        }
    } else {
        auto hackCommand = event->as<IHackCommand>();
        if (hackCommand) {
            auto it = hackConnections.find(hackCommand->getServerId());
            if (it != hackConnections.end())
                it->second.getEventQueue()->sendEvent(event);
        }
    }
    return true;
}
