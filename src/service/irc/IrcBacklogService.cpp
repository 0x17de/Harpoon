#include "IrcBacklogService.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "utils/ModuleProvider.hpp"

#include <iostream>


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
#pragma message "IrcBacklogService stub init"
            auto eventSetup = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventSetup->add(Database::Query(Database::QueryType::SetupTable,
                                                          "harpoon_irc_backlog"));
            query.add(Database::OperationType::Assign, "message_id", "id");
            query.add(Database::OperationType::Assign, "time", "time");
            query.add(Database::OperationType::Assign, "type", "int");
            query.add(Database::OperationType::Assign, "flags", "int");
            query.add(Database::OperationType::Join, "channel", "text", "harpoon_irc_backlog_channel");
            query.add(Database::OperationType::Join, "sender", "text", "harpoon_irc_backlog_sender");

            appQueue->sendEvent(eventSetup);
        } else if(eventType == EventDatabaseResult::uuid) {
            auto result = event->as<EventDatabaseResult>();
            bool success = result->getSuccess();
            UUID originType = result->getEventUuid();

            if (originType == EventInit::uuid) {
                if (success) {
                    for (auto e : heldBackEvents)
                        processEvent(e);
                    heldBackEvents.clear();
                    databaseInitialized = true;
                } else {
                    std::cout << "Error setting up irc backlog service. Service will be disabled" << std::endl;
                    getEventQueue()->setEnabled(false);
                    heldBackEvents.clear();
                    return false;
                }
            }
        } else {
            heldBackEvents.push_back(event);
        }
    } else {
#pragma message "IrcBacklogService stub"
    }

    return true;
}
