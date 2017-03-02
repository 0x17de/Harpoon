#include "HackConnection.hpp"
#include "HackConnection_Impl.hpp"
#include "HackChannelLoginData.hpp"
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
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <thread>
#include <chrono>

using namespace std;


HackConnection_Impl::HackConnection_Impl(EventQueue* appQueue,
                                       EventQueue* queue,
                                       size_t userId,
                                       const HackServerConfiguration& configuration)
    : appQueue{appQueue}
    , queue{queue}
    , userId{userId}
    , configuration{configuration}
    , running{true}
{
#pragma message "HackConnection_Impl: not yet implemented. initialize connection"
}

HackConnection_Impl::~HackConnection_Impl() {
    running = false;
#pragma message "~HackConnection_Impl: not yet implemented. initialize connection"
}

bool HackConnection_Impl::onEvent(std::shared_ptr<IEvent> event) {
    UUID type = event->getEventUuid();
    if (type == EventQuit::uuid) {
        running = false;
        cout << "[HC] received QUIT" << endl;
        return false;
    }
    return true;
}
