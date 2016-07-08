#include <iostream>
#include "LoginDatabase_Dummy.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLoginResult.hpp"
#include "utils/ModuleProvider.hpp"

using namespace std;


PROVIDE_MODULE("login_database", "dummy", LoginDatabase_Dummy);

LoginDatabase_Dummy::LoginDatabase_Dummy(EventQueue* appQueue)
    : EventLoop({
		  EventInit::uuid,
          EventQuit::uuid,
		  EventLogin::uuid
	  })
    , appQueue{appQueue}
{
}

bool LoginDatabase_Dummy::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "DB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventLogin::uuid) {
        auto login = event->as<EventLogin>();
        bool success = login->getUsername() == "user" && login->getPassword() == "password";
        appQueue->sendEvent(make_shared<EventLoginResult>(success, 1, login->getData()));
    }
    return true;
}

