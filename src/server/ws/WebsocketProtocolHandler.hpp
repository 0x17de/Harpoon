#ifndef WEBSOCKETPROTOCOLHANDLER_H
#define WEBSOCKETPROTOCOLHANDLER_H

#include <memory>
#include <json/json.h>


class IEvent;

/// Meant as an abstraction for handling different protocols in WebsocketHandler
class WebsocketProtocolHandler {
public:
    virtual ~WebsocketProtocolHandler();
    virtual void onData(Json::Value& root, std::shared_ptr<IEvent>& result) = 0;
    //virtual void onEvent(std::shared_ptr<IEvent> event) = 0;
};

#endif
