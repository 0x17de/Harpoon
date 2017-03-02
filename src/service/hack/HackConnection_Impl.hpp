#ifndef HACKCONNECTION_IMPL_H
#define HACKCONNECTION_IMPL_h

#include "event/hack/EventHackActivateService.hpp"
#include "HackConnection.hpp"
#include "HackChannelStore.hpp"
#include "HackServerConfiguration.hpp"
#include "HackServerHostConfiguration.hpp"
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>
#include <memory>
#include <thread>


class IEvent;
class EventQueue;
class HackChannelLoginData;
class HackConnection_Impl {
    static std::string getPureNick(const std::string& nick);
public:
    HackConnection_Impl(EventQueue* appQueue, EventQueue* queue, size_t userId, const HackServerConfiguration& configuration);
    ~HackConnection_Impl();

    static const std::map<char, char> prefixToMode;

    EventQueue* appQueue;
    EventQueue* queue;
    size_t userId;
    HackServerConfiguration configuration;
    bool running;
    //hack_session_t* hackSession;

    std::thread hackLoop;

    std::mutex channelLoginDataMutex;

    std::string nick;
    std::set<std::string> inUseNicks;
    std::map<std::string, HackChannelStore> channelStores;

    bool onEvent(std::shared_ptr<IEvent> event);
    bool findUnusedNick(std::string& nick);
};

#endif
