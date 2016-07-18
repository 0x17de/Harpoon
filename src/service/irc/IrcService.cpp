#include "IrcService.hpp"
#include "IrcChannelLoginData.hpp"
#include "IrcServerHostConfiguration.hpp"
#include "IrcServerConfiguration.hpp"
#include "IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventQuit.hpp"
#include "event/EventQueryChats.hpp"
#include "event/EventQuerySettings.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include "event/irc/EventIrcChatListing.hpp"
#include "event/irc/EventIrcSettingsListing.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
#include "event/irc/EventIrcServerDeleted.hpp"
#include "service/irc/IrcChannelStore.hpp"
#include <iostream>
#include <mutex>

using namespace std;


IrcService::IrcService(size_t userId, EventQueue* appQueue)
    : EventLoop({
          EventQuit::uuid,
          EventQueryChats::uuid,
          EventQuerySettings::uuid,
          EventIrcSendMessage::uuid,
          EventIrcServerAdded::uuid,
          EventIrcServerDeleted::uuid
      }, {
          &EventGuard<IActivateServiceEvent>
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
        for (auto& connection : ircConnections)
            connection.second.getEventQueue()->sendEvent(event);
        for (auto& connection : ircConnections)
            connection.second.join();
        return false;
    } else if (type == EventIrcServerAdded::uuid) {
        auto add = event->as<EventIrcServerAdded>();
        ircConnections.emplace(piecewise_construct,
                               forward_as_tuple(add->getServerId()),
                               forward_as_tuple(appQueue,
                                                add->getUserId(),
                                                (IrcServerConfiguration){add->getServerId(), add->getServerName()}));
    } else if (type == EventIrcServerDeleted::uuid) {
        auto del = event->as<EventIrcServerDeleted>();
        auto it = ircConnections.find(del->getServerId());
        if (it != ircConnections.end()) {
            auto& connection = it->second;
            connection.getEventQueue()->sendEvent(make_shared<EventQuit>());
            connection.join();
            ircConnections.erase(it);
        }
    } else if (type == EventQueryChats::uuid) {
        auto query = event->as<EventQueryChats>();

        auto listing = make_shared<EventIrcChatListing>(userId, query->getData());

        std::list<lock_guard<mutex>> locks;
        // lock all to assure correct results
        for (auto& cxnPair : ircConnections) {
            auto& connection = cxnPair.second;
            locks.emplace_back(connection.getChannelLoginDataMutex());
        }

        for (auto& cxnPair : ircConnections) {
            auto& connection = cxnPair.second;
            IrcServerListing& server = listing->addServer(connection.getServerId(),
                                                          connection.getServerName());
            for (auto& channelPair : connection.getChannelStore()) {
                string channelName = channelPair.first;
                const IrcChannelStore& channelStore = channelPair.second;
                IrcChannelListing& channel = server.addChannel(channelName);
                for (auto& userPair : channelStore.getUsers()) {
                    string username = userPair.second.getNick();
                    channel.addUser(username, "");
                }
            }
        }

        cout << "[US] Sending chat listing" << endl;
        appQueue->sendEvent(listing);
    } else if (type == EventQuerySettings::uuid) {
        auto query = event->as<EventQuerySettings>();

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
    } else if (type == EventIrcSendMessage::uuid) {
        auto message = event->as<EventIrcSendMessage>();
        auto it = ircConnections.find(message->getServerId());
        if (it != ircConnections.end())
            it->second.getEventQueue()->sendEvent(event);
    }
    return true;
}
