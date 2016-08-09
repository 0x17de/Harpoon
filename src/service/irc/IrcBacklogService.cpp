#include "IrcBacklogService.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseResult.hpp"
#include "utils/ModuleProvider.hpp"


PROVIDE_MODULE("irc_backlog", "default", IrcBacklogService);



IrcBacklogService::IrcBacklogService(EventQueue* appQueue)
    : appQueue{appQueue}
    , databaseInitialized{false}
{
}

IrcBacklogService::~IrcBacklogService() {
}

bool IrcBacklogService::onEvent(std::shared_ptr<IEvent> event) {
    return processEvent(event);
}

bool IrcBacklogService::processEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();

    if (eventType == EventQuit::uuid) {
        return false;
    }

    if (!databaseInitialized) {
        if (eventType == EventInit::uuid) {
#warning IrcBacklogService stub init
        } else if(eventType == EventDatabaseResult::uuid) {
            for (auto e : heldBackEvents)
                processEvent(e);
            heldBackEvents.clear();
        } else {
            heldBackEvents.push_back(event);
        }
    } else {
#warning IrcBacklogService stub
    }

    return true;
}
