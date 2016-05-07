#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include "ApplicationGuard.hpp"
#include "queue/EventLoop.hpp"

class IEvent;
class Application : public EventLoop {
	ApplicationGuard guard;
	std::shared_ptr<EventLoop> userManager;
	std::shared_ptr<EventLoop> database;
	std::shared_ptr<EventLoop> server1;
public:
	Application();
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
