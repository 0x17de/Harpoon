#include <iostream>
#include "UserManager.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/IActivateServiceEvent.hpp"

using namespace std;


UserManager::UserManager(EventQueue* appQueue)
    : EventLoop({
        EventQuit::uuid,
        EventLoginResult::uuid
      }, {
          &EventGuard<IActivateServiceEvent>
        , &EventGuard<IUserEvent>
      })
    , appQueue{appQueue}
{
}

UserManager::~UserManager() {
}

bool UserManager::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();

	auto userEvent = event->as<IUserEvent>();
	if (userEvent) {
		size_t userId = userEvent->getUserId();

		auto activateEvent = event->as<IActivateServiceEvent>();
		if (activateEvent) {
			std::cout << "UM received Activate event" << std::endl;
			cout << "[UM] Activate user: " << activateEvent->getUserId() << endl;
			std::map<size_t, std::shared_ptr<EventLoop>>* serviceMap;
			auto serviceMapIt = users.find(userId);
			if (serviceMapIt == users.end()) {
				auto it = users.emplace(piecewise_construct,
					forward_as_tuple(userId),
					forward_as_tuple());
				serviceMap = &it.first->second;
			} else {
				serviceMap = &serviceMapIt->second;
			}
			auto serviceIt = serviceMap->find(eventType);
			if (serviceIt == serviceMap->end()) {
				auto serviceInstanceIt = serviceMap->emplace(eventType, activateEvent->instantiateService(userId, appQueue));
				serviceInstanceIt.first->second->getEventQueue()->sendEvent(event);
			}
		} else if (eventType == EventLoginResult::uuid) {
			std::cout << "UM received User login result event" << std::endl;
			auto loginResult = event->as<EventLoginResult>();
			if (loginResult->getSuccess()) {
				auto it = users.find(userId);
				if (it != users.end()) {
					for (auto p : it->second) {
						auto queue = p.second->getEventQueue();
						if (queue->canProcessEvent(event.get()))
							queue->sendEvent(event);
					}
				}
			}
		} else {
			auto it = users.find(userId);
			if (it != users.end()) {
				for (auto p : it->second) {
					auto queue = p.second->getEventQueue();
					if (queue->canProcessEvent(event.get()))
						p.second->getEventQueue()->sendEvent(event);
				}
			}
		}
	}

	if (eventType == EventQuit::uuid) {
		cout << "[UM] received QUIT event" << endl;
		users.clear();
		return false;
	}

	return true;
}

