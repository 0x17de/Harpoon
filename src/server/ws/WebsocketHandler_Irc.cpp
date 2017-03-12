#include "WebsocketHandler_Irc.hpp"

PROVIDE_WEBSOCKETPROTOCOL_MODULE("websocketprotocol", "irc", WebsocketHandler_Irc)


WebsocketHandler_Irc::~WebsocketHandler_Irc() = default;

void WebsocketHandler_Irc::onData(Json::Value& root, std::shared_ptr<IEvent>& result) {
#pragma handle websocket irc onData
}
