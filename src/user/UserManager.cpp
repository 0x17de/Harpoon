#include <iostream>
#include "UserManager.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/IActivateUserEvent.hpp"

using namespace std;


UserManager::UserManager(EventQueue* appQueue) :
	ManagingEventLoop({
		EventQuit::uuid,
		EventLoginResult::uuid,
	}, {
		&EventGuard<IActivateUserEvent>
	}),
	appQueue{appQueue}
{
}

void UserManager::sendEventToUser(std::shared_ptr<IEvent> event) {
	auto userEvent = event->as<IUserEvent>();
	if (userEvent != nullptr) {
		cout << "NonNULL event" << endl;
		auto it = users.find(userEvent->getUserId());
		if (it != users.end()) {
			EventQueue* queue = it->second->getEventQueue();
			if (queue->canProcessEvent(event.get()))
				queue->sendEvent(event);
		}
	}
}

bool UserManager::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();

	auto userEvent = event->as<IUserEvent>();
	if (userEvent) {
		std::cout << "UM received User event" << std::endl;
		size_t userId = userEvent->getUserId();

		auto activateEvent = event->as<IActivateUserEvent>();
		if (activateEvent) {
			std::cout << "UM received Activate event" << std::endl;
			cout << "[UM] Activate user: " << activateEvent->getUserId() << endl;
			auto it = users.emplace(userId, activateEvent->instantiateService(userId, appQueue));
			it.first->second->getEventQueue()->sendEvent(event);
		}
	}

	if (eventType == EventQuit::uuid) {
		cout << "[UM] received QUIT event" << endl;
		for (auto& user : users)
			user.second->getEventQueue()->sendEvent(event);
		for (auto& user : users)
			user.second->join();
		return false;
	}

	return true;
}
