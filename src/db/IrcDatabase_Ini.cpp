#include "IrcDatabase_Ini.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "service/irc/IrcChannelLoginData.hpp"
#include "service/irc/IrcServerHostConfiguration.hpp"
#include "service/irc/IrcServerConfiguration.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include <iostream>
#include <sstream>

using namespace std;


IrcDatabase_Ini::IrcDatabase_Ini(EventQueue* appQueue) :
    EventLoop({
        EventInit::uuid,
        EventQuit::uuid,
        EventLoginResult::uuid
    }),
    appQueue{appQueue}
{
}

bool IrcDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "IrcDB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventInit::uuid) {
        std::cout << "IrcDB received INIT event" << std::endl;

        size_t userId = 1;
        auto login = make_shared<EventIrcActivateService>(userId);

        stringstream configfile;
        configfile << "config/" << userId << "/irc.servers.ini";
        
        /*
        Ini ini("{...}.ini");
        if (!ini.isNew()) {
            ini.getEntries()
        }
        */

        appQueue->sendEvent(login);
    }
    return true;
}

