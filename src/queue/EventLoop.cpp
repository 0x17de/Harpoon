#include "EventLoop.h"
#include "queue/EventQueue.h"

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
	std::shared_ptr<IEvent> event;

	int eventResult;
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

