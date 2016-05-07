#include "EventLoop.hpp"
#include "queue/EventQueue.hpp"

EventLoop::EventLoop() :
	t{std::thread(go, this)}
{
}

EventLoop::EventLoop(std::set<UUID> processableEvents) :
	queue{processableEvents},
	t{std::thread(go, this)}
{
}

void EventLoop::go(EventLoop* loop) {
	loop->run();
}

void EventLoop::run() {
	std::shared_ptr<IEvent> event; // will be filled by getEvent

	int eventResult;
	// loop and wait for events with a timeout of 1000
	while ((eventResult = queue.getEvent(1000, event)) >= 0) {
		if (eventResult < 0) break; // error
		if (eventResult == 0) continue; // timeout
		if (!onEvent(event)) break;
	}
}

EventQueue* EventLoop::getEventQueue() {
	return &queue;
}

void EventLoop::join() {
	t.join();
}

