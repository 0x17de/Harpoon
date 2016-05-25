#include <iostream>
#include "UserManager.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/EventActivateUser.hpp"

using namespace std;


UserManager::UserManager(EventQueue* appQueue) :
	ManagingEventLoop({
		EventQuit::uuid,
		EventLoginResult::uuid,
		EventActivateUser::uuid
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
			EventQueue* queue = it->second.getEventQueue();
			if (queue->canProcessEvent(event->getEventUuid()))
				queue->sendEvent(event);
		}
	}
}

bool UserManager::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();

	if (eventType == EventQuit::uuid) {
		cout << "[UM] received QUIT event" << endl;
		for (auto& user : users)
			user.second.getEventQueue()->sendEvent(event);
		for (auto& user : users)
			user.second.join();
		return false;
	} else if (eventType == EventActivateUser::uuid) {
		auto activateEvent = event->as<EventActivateUser>();
		cout << "[UM] Activate user: " << activateEvent->getUserId() << endl;
		size_t userId = activateEvent->getUserId();
		auto it = users.emplace(std::piecewise_construct,
			std::forward_as_tuple(userId),
			std::forward_as_tuple(userId, appQueue));
		it.first->second.getEventQueue()->sendEvent(event);
	}
	return true;
}
