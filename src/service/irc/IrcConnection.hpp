#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "queue/EventLoop.hpp"
#include <memory>
#include <mutex>
#include <map>
#include <string>


class IrcServerConfiguration;
class IrcChannelStore;
class EventQueue;
class IrcConnection_Impl;
class IrcConnection : public EventLoop {
    std::shared_ptr<IrcConnection_Impl> impl;
public:
    IrcConnection(EventQueue* appQueue, size_t userId, const IrcServerConfiguration& configuration);
    virtual ~IrcConnection();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;

    size_t getServerId() const;
    std::string getServerName() const;
    std::mutex& getChannelLoginDataMutex() const;
    const std::map<std::string, IrcChannelStore>& getChannelStore() const;
    const IrcChannelStore* getChannelStore(const std::string& channelName) const;
};

#endif
