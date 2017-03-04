#ifndef WEBSOCKETHANDLER_IRC_H
#define WEBSOCKETHANDLER_IRC_H

#include "WebsocketProtocolHandler.hpp"


class WebsocketHandler_Irc : public WebsocketProtocolHandler {
public:
    void onData(Json::Value& root, std::std::shared_ptr<IEvent> result);
};

#endif
