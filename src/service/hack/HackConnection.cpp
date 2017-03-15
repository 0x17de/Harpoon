#include <iostream>
#include "HackConnection.hpp"
#include "HackChannelLoginData.hpp"
#include "HackChannelStore.hpp"
#include "event/EventQuit.hpp"
#include "event/IActivateServiceEvent.hpp"
#include "event/hack/EventHackJoinChannel.hpp"
#include "event/hack/EventHackJoined.hpp"
#include "event/hack/EventHackParted.hpp"
#include "event/hack/EventHackPartChannel.hpp"
#include "event/hack/EventHackDeleteChannel.hpp"
#include "event/hack/EventHackSendMessage.hpp"
#include "event/hack/EventHackMessage.hpp"
#include "event/hack/EventHackModifyNick.hpp"
#include "event/hack/EventHackNickModified.hpp"
#include "event/hack/EventHackUserlistReceived.hpp"

using namespace std;


HackConnection::HackConnection(EventQueue* appQueue,
                               size_t userId,
                               const HackServerConfiguration& configuration)
    : EventLoop({
          EventQuit::uuid
      }, {
          EventGuard<IUserEvent>
      })
    , userId{userId}
    , configuration{configuration}
    , running{true}
{
}

HackConnection::~HackConnection() {
}

bool HackConnection::onEvent(std::shared_ptr<IEvent> event) {
    UUID type = event->getEventUuid();
    if (type == EventQuit::uuid) {
        running = false;
        std::cout << "[HC] received QUIT" << std::endl;
        return false;
    }
    return true;
}

std::string HackConnection::getActiveNick() const {
    return nick;
}

size_t HackConnection::getServerId() const {
    return configuration.getServerId();
}

void HackConnection::addHost(const std::string& hostName,
                             const std::string& websocketUri,
                             int port,
                             bool ipV6,
                             bool ssl) {
    configuration.addHostConfiguration(hostName,
                                             websocketUri,
                                             port,
                                             ipV6,
                                             ssl);
}

void HackConnection::removeHost(const std::string& host, int port) {
    configuration.removeHost(host, port);
}


const HackServerConfiguration& HackConnection::getServerConfiguration() const {
    return configuration;
}

std::string HackConnection::getServerName() const {
    return configuration.getServerName();
}

std::mutex& HackConnection::getChannelLoginDataMutex() const {
    return channelLoginDataMutex;
}

const std::map<std::string, HackChannelStore>& HackConnection::getChannelStore() const {
    return channelStores;
}

const HackChannelStore* HackConnection::getChannelStore(const std::string& channelName) const {
    auto it = channelStores.find(channelName);
    return it == channelStores.end() ? 0 : &it->second;
}
