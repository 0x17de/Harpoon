#include <iostream>
#include "IrcDatabase_Dummy.hpp"
#include "event/EventInit.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLoginResult.hpp"
#include "event/irc/EventIrcActivateService.hpp"
#include "event/irc/EventIrcJoinChannel.hpp" // IrcChannelLoginData

using namespace std;


IrcDatabase_Dummy::IrcDatabase_Dummy(EventQueue* appQueue) :
	EventLoop({
		EventInit::uuid,
		EventQuit::uuid,
		EventLoginResult::uuid
	}),
	appQueue{appQueue}
{
}

bool IrcDatabase_Dummy::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		std::cout << "IrcDB received QUIT event" << std::endl;
		return false;
	} else if (eventType == EventInit::uuid) {
		std::cout << "IrcDB received INIT event" << std::endl;

		size_t userId = 1;
		auto login = make_shared<EventIrcActivateService>(userId);

		size_t serverId = 1;
		auto& loginConfiguration = login->addLoginConfiguration(serverId,
			"TestServer",
			"127.0.0.1",
			6667,
			"wealllikedebian",
			false,
			false);

		loginConfiguration.addNick("iirc");
		loginConfiguration.addNick("iirc2");
		loginConfiguration.addNick("iirc3");
		loginConfiguration.addChannelLoginData(1, "#test", "");
		loginConfiguration.addChannelLoginData(2, "#test2", "");

		appQueue->sendEvent(login);
	}
	return true;
}

