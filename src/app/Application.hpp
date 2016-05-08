#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <list>
#include "ApplicationGuard.hpp"
#include "queue/EventLoop.hpp"

class UserManager;
class IEvent;
class Application : public EventLoop {
	ApplicationGuard guard;
	std::shared_ptr<UserManager> userManager;
	std::list<std::shared_ptr<EventLoop>> eventHandlers;
public:
	Application();
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
