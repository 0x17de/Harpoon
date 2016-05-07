#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <thread>
#include <memory>
#include <set>
#include "utils/uuid.hpp"
#include "queue/EventQueue.hpp"


class IEvent;
class EventQueue;
class EventLoop {
	EventQueue queue;
	std::thread t;
	static void go(EventLoop* loop);
	void run();
protected:
	virtual bool onEvent(std::shared_ptr<IEvent> event) = 0;
public:
	EventLoop();
	EventLoop(std::set<UUID> processableEvents);
	EventQueue* getEventQueue();
	void join();
};

#endif
