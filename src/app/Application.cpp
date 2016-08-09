#include <iostream>
#include <csignal>
#include <list>

#include "Application.hpp"
#include "utils/Ini.hpp"
#include "queue/EventQueue.hpp"
#include "user/UserManager.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "utils/ModuleProvider.hpp"

using namespace std;


Application::Application()
    : guard{this}
    , EventLoop({}, {}, false)
{
    Ini coreIni("config/core.ini");
    if (coreIni.isNew()) {
        cerr << "No configuration exists. See --help for setup." << endl;
        stop();
        return;
    }
    auto& modules = coreIni.expectCategory("modules");
    auto& services = coreIni.expectCategory("services");

    EventQueue* queue = getEventQueue();
    userManager = make_shared<UserManager>(queue);
    eventHandlers.push_back(userManager);

    string loginDatabaseType,
        enableWebChat,
        databaseType;

    coreIni.getEntry(modules, "login", loginDatabaseType);
    coreIni.getEntry(modules, "webchat", enableWebChat);
    coreIni.getEntry(modules, "database", databaseType);

    auto& moduleProvider = ModuleProvider::getInstance();
    eventHandlers.push_back(moduleProvider.initializeModule("login_database", loginDatabaseType, queue));

    // load irc settings
    string enableIrcService;
    coreIni.getEntry(services, "irc", enableIrcService);
    if (enableIrcService == "y") {
        Ini ircIni("config/irc.ini");
        auto& modules = ircIni.expectCategory("modules");

        string ircDatabaseType,
            backlogEnabled;
        ircIni.getEntry(modules, "settings_database", ircDatabaseType);
        ircIni.getEntry(modules, "backlog", backlogEnabled);

        eventHandlers.push_back(moduleProvider.initializeModule("irc_database", ircDatabaseType, queue));
        if (backlogEnabled == "y")
            eventHandlers.push_back(moduleProvider.initializeModule("irc_backlog", "default", queue));
    }

    if (databaseType != "" && databaseType != "none")
        eventHandlers.push_back(moduleProvider.initializeModule("database", databaseType, queue));

#ifdef USE_WEBSOCKET_SERVER
    if (enableWebChat == "y")
        eventHandlers.push_back(moduleProvider.initializeModule("server", "websocket", queue));
#endif

    for (auto& eventHandler : eventHandlers)
        eventHandler->getEventQueue()->sendEvent(make_shared<EventInit>());

    run();
}

Application::~Application() {
}

void Application::stop() {
    getEventQueue()->sendEvent(make_shared<EventQuit>());
}

bool Application::onEvent(std::shared_ptr<IEvent> event) {
    // which event do we process?
    UUID eventType = event->getEventUuid();

    for (auto& eventHandler : eventHandlers) {
        auto eventQueue = eventHandler->getEventQueue();
        if (eventQueue->canProcessEvent(event.get()))
            eventQueue->sendEvent(event);
    }

    if (eventType == EventQuit::uuid) {
        cout << "Received Quit Event" << endl;
        eventHandlers.clear();
        cout << "Submodules were stopped" << endl;
        return false;
    }
    return true;
}

