#include "ApplicationGuard.h"
#include <memory>
#include <csignal>
#include "Application.h"
#include "event/EventQuit.h"

using namespace std;


static void sigInt(int) {
	for (auto app : ApplicationGuard::apps)
		app->getEventQueue()->sendEvent(make_shared<EventQuit>());
}

std::list<Application*> ApplicationGuard::apps;
ApplicationGuard::ApplicationGuard(Application* app) : app(app) {
	signal(SIGTERM, sigInt);
	signal(SIGINT, sigInt);
	apps.push_back(app);
}
ApplicationGuard::~ApplicationGuard() {
	apps.remove(app);
}

