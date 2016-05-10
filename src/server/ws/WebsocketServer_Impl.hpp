#ifndef WEBSOCKETSERVER_IMPL_H
#define WEBSOCKETSERVER_IMPL_H
#include <memory>


class EventQueue;
class WebsocketServer_Impl {
public:
	EventQueue* appQueue;

	WebsocketServer_Impl(EventQueue* appQueue);
	bool onEvent(std::shared_ptr<IEvent> event);
};

#endif
