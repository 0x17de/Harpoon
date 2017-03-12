#ifndef WEBSOCKETHANDLER_IRC_H
#define WEBSOCKETHANDLER_IRC_H

#include "WebsocketProtocolHandler.hpp"
#include "utils/ModuleProvider.hpp"


class WebsocketHandler_Irc : public WebsocketProtocolHandler {
public:
    virtual ~WebsocketHandler_Irc();
    void onData(Json::Value& root, std::shared_ptr<IEvent>& result) override;
};

#endif
