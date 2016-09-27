#include "IrcBacklogService.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "utils/ModuleProvider.hpp"

#include <iostream>
#include <ctime>


PROVIDE_MODULE("irc_backlog", "default", IrcBacklogService);



IrcBacklogService::IrcBacklogService(EventQueue* appQueue)
    : EventLoop({
                    EventDatabaseQuery::uuid
                  , EventQuit::uuid
                },
                {
                    &EventGuard<IIrcLoggable>
                })
    , appQueue{appQueue}
    , databaseInitialized{false}
{
}

IrcBacklogService::~IrcBacklogService() {
}

bool IrcBacklogService::onEvent(std::shared_ptr<IEvent> event) {
    return processEvent(event);
}

std::string IrcBacklogService::convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp) {
    std::time_t t = std::chrono::system_clock::to_time_t(timestamp);
    return std::ctime(&t);
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
            query.add(Database::OperationType::Assign, "message", "text");
            query.add(Database::OperationType::Assign, "type", "int");
            query.add(Database::OperationType::Assign, "flags", "int");
            query.add(Database::OperationType::Join, "channel", "text", "harpoon_irc_channel");
            query.add(Database::OperationType::Join, "sender", "text", "harpoon_irc_sender");

            appQueue->sendEvent(eventSetup);
        } else if(eventType == EventDatabaseResult::uuid) {
            auto result = event->as<EventDatabaseResult>();
            bool success = result->getSuccess();
            UUID originType = result->getEventOrigin()->getEventUuid();

            if (originType == EventInit::uuid) {
                if (success) {
                    databaseInitialized = true;
                    for (auto e : heldBackEvents)
                        processEvent(e);
                    heldBackEvents.clear();
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
        if (eventType == EventIrcMessage::uuid) {
            auto message = event->as<EventIrcMessage>();
            auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                           "harpoon_irc_backlog",
                                                           std::list<std::string>{"time", "message", "type", "flags", "channel_ref", "sender_ref"}));
            query.add(Database::OperationType::Assign, convertTimestamp(message->getTimestamp()));
            query.add(Database::OperationType::Assign, message->getMessage());
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "", "", "0");
            query.add(Database::OperationType::Assign, "", "", "1");
            query.add(Database::OperationType::Join, "channel", message->getChannel(), "harpoon_irc_channel");
            query.add(Database::OperationType::Join, "sender", message->getFrom(), "harpoon_irc_sender");

            appQueue->sendEvent(eventInsert);
        } else if (eventType == EventIrcJoined::uuid) {
            auto joined = event->as<EventIrcJoined>();
            auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                           "harpoon_irc_backlog",
                                                           std::list<std::string>{"time", "type", "flags", "channel_ref", "sender_ref"}));
            query.add(Database::OperationType::Assign, convertTimestamp(joined->getTimestamp()));
            query.add(Database::OperationType::Assign, "1");
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "", "", "0");
            query.add(Database::OperationType::Assign, "", "", "1");
            query.add(Database::OperationType::Join, "channel", joined->getChannel(), "harpoon_irc_channel");
            query.add(Database::OperationType::Join, "sender", joined->getUsername(), "harpoon_irc_sender");

            appQueue->sendEvent(eventInsert);
        } else if (eventType == EventIrcParted::uuid) {
            auto parted = event->as<EventIrcParted>();
            auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                           "harpoon_irc_backlog",
                                                           std::list<std::string>{"time", "type", "flags", "channel_ref", "sender_ref"}));
            query.add(Database::OperationType::Assign, convertTimestamp(parted->getTimestamp()));
            query.add(Database::OperationType::Assign, "2");
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "", "", "0");
            query.add(Database::OperationType::Assign, "", "", "1");
            query.add(Database::OperationType::Join, "channel", parted->getChannel(), "harpoon_irc_channel");
            query.add(Database::OperationType::Join, "sender", parted->getUsername(), "harpoon_irc_sender");

            appQueue->sendEvent(eventInsert);
        } else if (eventType == EventIrcQuit::uuid) {
            auto quit = event->as<EventIrcQuit>();
            auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                           "harpoon_irc_backlog",
                                                           std::list<std::string>{"time", "type", "flags", "sender_ref"}));
            query.add(Database::OperationType::Assign, convertTimestamp(quit->getTimestamp()));
            query.add(Database::OperationType::Assign, "3");
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "", "", "0");
            query.add(Database::OperationType::Assign, "", "", "1");
            query.add(Database::OperationType::Join, "sender", quit->getWho(), "harpoon_irc_sender");

            appQueue->sendEvent(eventInsert);
        } else if (eventType == EventIrcKicked::uuid) {
            auto kicked = event->as<EventIrcKicked>();
            auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
            auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                           "harpoon_irc_backlog",
                                                           std::list<std::string>{"time", "message", "type", "flags", "channel_ref", "sender_ref"}));
            query.add(Database::OperationType::Assign, convertTimestamp(kicked->getTimestamp()));
            query.add(Database::OperationType::Assign, kicked->getTarget());
            query.add(Database::OperationType::Assign, "4");
            query.add(Database::OperationType::Assign, "0");
            query.add(Database::OperationType::Assign, "", "", "0");
            query.add(Database::OperationType::Assign, "", "", "1");
            query.add(Database::OperationType::Join, "channel", kicked->getChannel(), "harpoon_irc_channel");
            query.add(Database::OperationType::Join, "sender", kicked->getUsername(), "harpoon_irc_sender");

            appQueue->sendEvent(eventInsert);
        }
    }

    return true;
}
