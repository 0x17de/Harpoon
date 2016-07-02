#include <iostream>
#include <csignal>
#include <list>

#include "Application.hpp"
#include "utils/Ini.hpp"
#include "queue/EventQueue.hpp"
#include "user/UserManager.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "db/LoginDatabase_Dummy.hpp"
#include "db/LoginDatabase_Ini.hpp"
#include "db/IrcDatabase_Dummy.hpp"
#include "server/ws/WebsocketServer.hpp"
#include "utils/ModuleProvider.hpp"

using namespace std;


Application::Application()
    : guard{this}
    , EventLoop()
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
		enableIrcService;

	coreIni.getEntry(modules, "login", loginDatabaseType);
	coreIni.getEntry(modules, "webchat", enableWebChat);
	coreIni.getEntry(services, "irc", enableIrcService);

	auto& moduleProvider = ModuleProvider::getInstance();
	eventHandlers.push_back(moduleProvider.initializeModule("login_database", loginDatabaseType, queue));

	eventHandlers.push_back(make_shared<IrcDatabase_Dummy>(queue));

#ifdef USE_WEBSOCKET_SERVER
	if (enableWebChat == "y")
		eventHandlers.push_back(make_shared<WebsocketServer>(queue));
#endif

	for (auto& eventHandler : eventHandlers)
		eventHandler->getEventQueue()->sendEvent(make_shared<EventInit>());
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

        // send events to managed subqueues
        auto userEventLoop = dynamic_cast<ManagingEventLoop*>(eventHandler.get());
        if (userEventLoop != nullptr)
            userEventLoop->sendEventToUser(event); // sends only to matching user (only if IUserEvent)
    }

    if (eventType == EventQuit::uuid) {
        cout << "Received Quit Event" << endl;
        for (auto& eventHandler : eventHandlers)
            eventHandler->join();
        cout << "Submodules were stopped" << endl;
        return false;
    }
    return true;
}

