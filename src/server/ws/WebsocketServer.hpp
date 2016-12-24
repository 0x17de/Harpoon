#ifdef USE_WEBSOCKET_SERVER
#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include "queue/EventLoop.hpp"
#include <memory>


class EventQueue;
class WebsocketServer_Impl;
class WebsocketServer : public EventLoop {
    std::shared_ptr<WebsocketServer_Impl> impl;
public:
    explicit WebsocketServer(EventQueue* appQueue);
    virtual ~WebsocketServer();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
#endif
