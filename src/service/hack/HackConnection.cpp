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
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include "utils/Cpp11Utils.hpp"

using namespace std;
namespace ws = websocketpp;

typedef ws::client<ws::config::asio_client> WebsocketClient;
typedef ws::lib::shared_ptr<ws::lib::thread> WebsocketThread;


class HackWebsocketEndpoint {
public:
    HackWebsocketEndpoint();

private:
    WebsocketClient _endpoint;
    WebsocketThread _thread;
};

HackWebsocketEndpoint::HackWebsocketEndpoint() {
    _endpoint.clear_access_channels(ws::log::alevel::all);
    _endpoint.clear_error_channels(ws::log::elevel::all);

    _endpoint.init_asio();
    _endpoint.start_perpetual();

    _thread = ws::lib::make_shared<ws::lib::thread>(&WebsocketClient::run, &_endpoint);
}


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
    , connected{false}
    , hostIndex{0}
    , hackEndpoint{cpp11::make_unique<HackWebsocketEndpoint>()}
{
    connect();
}

HackConnection::~HackConnection() = default;


void HackConnection::connect() {
    if (connected) return;

    HackServerHostConfiguration hostConfiguration;
    {
        lock_guard<mutex> lock(channelLoginDataMutex);
        channelStores.clear();
        for (auto& channel : configuration.getChannelLoginData()) {
            channelStores.emplace(piecewise_construct,
                                  forward_as_tuple(channel.getChannelName()),
                                  forward_as_tuple(channel.getDisabled()));
        }

        auto& hostConfigurations = configuration.getHostConfigurations();
        if (hostConfigurations.empty()) return;
        hostIndex %= hostConfigurations.size();

        hostConfiguration = *next(hostConfigurations.begin(), hostIndex);
        //if (!findUnusedNick(nick)) break;
    }

    ostringstream urlOs;
    urlOs
        << (hostConfiguration.getSsl() ? "wss://" : "ws://")
        << hostConfiguration.getHostName()
        << hostConfiguration.getWebsocketUri();

    // TODO: continue
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
