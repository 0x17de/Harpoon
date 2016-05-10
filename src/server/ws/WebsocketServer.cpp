#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/EventQuit.hpp"

using namespace std;


WebsocketServer::WebsocketServer(EventQueue* appQueue)
{
}

WebsocketServer::~WebsocketServer() {
}

bool WebsocketServer::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		return false;
	}
	return true;
}

void WebsocketServer::sendEventToUser(std::shared_ptr<IEvent> event) {

}

