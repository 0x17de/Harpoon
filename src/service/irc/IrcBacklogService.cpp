#include "IrcBacklogService.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "utils/ModuleProvider.hpp"


PROVIDE_MODULE("irc_backlog", "default", IrcBacklogService);

IrcBacklogService::IrcBacklogService(EventQueue* appQueue)
    : appQueue{appQueue}
{
}

bool IrcBacklogService::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();

    if (eventType == EventQuit::uuid) {
        return false;
    } else if (eventType == EventInit::uuid) {
#warning IrcBacklogService stub init
    }
#warning IrcBacklogService stub

    return true;
}
