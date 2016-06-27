#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"

using namespace std;


WebsocketClientData::WebsocketClientData(size_t userId, seasocks::WebSocket* socket)
    : userId{userId}
    , socket{socket}
{
}


WebsocketServer::WebsocketServer(EventQueue* appQueue)
    : impl{make_shared<WebsocketServer_Impl>(getEventQueue(), appQueue)}
{
}

WebsocketServer::~WebsocketServer() {
}

bool WebsocketServer::onEvent(std::shared_ptr<IEvent> event) {
    return impl->onEvent(event);
}

void WebsocketServer::sendEventToUser(std::shared_ptr<IEvent> event) {
    impl->sendEventToUser(event);
}

