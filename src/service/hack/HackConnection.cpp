#include <iostream>
#include <thread>
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
#include "utils/Cpp11Utils.hpp"

using namespace std;
namespace ws = websocketpp;


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
{
    initializeEndpoint();
    connect();
}

HackConnection::~HackConnection() {
    if (_thread) {
        _endpoint->stop_perpetual();
        _endpoint->stop();
        _thread->join();
    }
}


void HackConnection::initializeEndpoint() {
    _endpoint = cpp11::make_unique<WebsocketClient>();

    _endpoint->clear_access_channels(ws::log::alevel::all);
    _endpoint->clear_error_channels(ws::log::elevel::all);

    _endpoint->init_asio();
    _endpoint->start_perpetual();

    _thread = cpp11::make_unique<std::thread>(&WebsocketClient::run, _endpoint.get());
}

bool HackConnection::findUnusedNick(std::string& nick) {
    auto& nicks = configuration.getNicks();
    auto nickIt = find_if(nicks.begin(), nicks.end(), [this](const std::string& foundNick) {
        return inUseNicks.find(foundNick) == inUseNicks.end(); // not used
    });
    bool success = nickIt != nicks.end();
    if (success) nick = *nickIt; // assign nick
    return success;
}

void HackConnection::connect() {
    if (connected) return;

    HackServerHostConfiguration hostConfiguration;
    std::string nick;

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
        if (!findUnusedNick(nick)) return;
    }

    ostringstream urlOs;
    urlOs
        << (hostConfiguration.getSsl() ? "wss://" : "ws://")
        << hostConfiguration.getHostName()
        << ":"
        << hostConfiguration.getPort()
        << hostConfiguration.getWebsocketUri();

    // websocket startup
    websocketpp::lib::error_code ec;
    WebsocketClient::connection_ptr con = _endpoint->get_connection(urlOs.str(), ec);

    if (ec) {
        std::cerr << "> Connect initialization error: " << ec.message() << std::endl;
        return;
    }

    con->set_open_handler([this](ws::connection_hdl hdl){
        // TODO
    });
    con->set_fail_handler([this](ws::connection_hdl hdl){
        // TODO
    });
    con->set_message_handler([this](ws::connection_hdl hdl,
                                    WebsocketClient::message_ptr message){
        // TODO
    });

    _endpoint->connect(con);
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
