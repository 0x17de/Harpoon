#include <iostream>
#include "LoginDatabase_Ini.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLoginResult.hpp"
#include "utils/Password.hpp"
#include "utils/ModuleProvider.hpp"

using namespace std;


PROVIDE_EVENTLOOP_MODULE("login_database", "ini", LoginDatabase_Ini)

LoginDatabase_Ini::LoginDatabase_Ini(EventQueue* appQueue)
    : EventLoop({
          EventInit::uuid,
          EventQuit::uuid,
          EventLogin::uuid
      })
    , appQueue{appQueue}
    , usersIni{"config/users.ini"}
{
}

LoginDatabase_Ini::~LoginDatabase_Ini() {
}

bool LoginDatabase_Ini::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();
    if (eventType == EventQuit::uuid) {
        std::cout << "DB received QUIT event" << std::endl;
        return false;
    } else if (eventType == EventLogin::uuid) {
        auto login = event->as<EventLogin>();

        bool success;
        Ini::Entries* user = usersIni.getEntry(login->getUsername());
        if (user) {
            string salt, password;
            success = (
                       usersIni.getEntry(*user, "salt", salt)
                       && usersIni.getEntry(*user, "password", password)
                       && Password(salt, login->getPassword()).equals(password)
                       );
        } else {
            success = false;
        }

        appQueue->sendEvent(make_shared<EventLoginResult>(success, 1, login->getData()));
    }
    return true;
}

