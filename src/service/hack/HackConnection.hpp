#ifndef HACKCONNECTION_H
#define HACKCONNECTION_H

#include "queue/EventLoop.hpp"
#include <memory>
#include <mutex>
#include <map>
#include <string>
#include "service/hack/HackServerConfiguration.hpp"


class HackWebsocketEndpoint;
class HackServerConfiguration;
class HackChannelStore;
class EventQueue;
class HackConnection_Impl;
class HackConnection : public EventLoop {
    static const std::map<char, char> prefixToMode;

    size_t userId;
    HackServerConfiguration configuration;
    bool running;
    std::unique_ptr<HackWebsocketEndpoint> hackEndpoint;

    std::thread hackLoop;

    mutable std::mutex channelLoginDataMutex;

    std::string nick;
    std::set<std::string> inUseNicks;
    std::map<std::string, HackChannelStore> channelStores;
public:
    HackConnection(EventQueue* appQueue, size_t userId, const HackServerConfiguration& configuration);
    virtual ~HackConnection();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;

    std::string getActiveNick() const;
    size_t getServerId() const;
    std::string getServerName() const;
    std::mutex& getChannelLoginDataMutex() const;
    void addHost(const std::string& hostName,
                 const std::string& websocketUri,
                 int port,
                 bool ipV6,
                 bool ssl);
    void removeHost(const std::string& host, int port);
    const HackServerConfiguration& getServerConfiguration() const;
    const std::map<std::string, HackChannelStore>& getChannelStore() const;
    const HackChannelStore* getChannelStore(const std::string& channelName) const;
};

#endif
