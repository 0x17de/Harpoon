#include "IrcBacklogService.hpp"
#include "db/query/Database_Query.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/irc/EventIrcServiceInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "event/irc/EventIrcMessageType.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcRequestBacklog.hpp"
#include "utils/IdProvider.hpp"
#include "utils/ModuleProvider.hpp"

#include <iostream>
#include <ctime>
#include <string>
#include <sstream>


PROVIDE_MODULE("irc_backlog", "default", IrcBacklogService);



IrcBacklogService::IrcBacklogService(EventQueue* appQueue)
    : EventLoop({
                    EventDatabaseQuery::uuid
                  , EventQuit::uuid
                },
                {
                    &EventGuard<IrcLoggable>
                })
    , appQueue{appQueue}
    , databaseInitialized{false}
    , lastIdFetched{false}
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

void IrcBacklogService::setupTable(std::shared_ptr<IEvent> event) {
    //auto eventSetup = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
    // TODO: create table
    /*auto& query = eventSetup->add(Database::Query(Database::QueryType::SetupTable,
                                                  "harpoon_irc_backlog"));
    query.add(Database::OperationType::Assign, "message_id", "id");
    query.add(Database::OperationType::Assign, "time", "time");
    query.add(Database::OperationType::Assign, "message", "text");
    query.add(Database::OperationType::Assign, "type", "int");
    query.add(Database::OperationType::Assign, "flags", "int");
    query.add(Database::OperationType::Join, "channel", "text", "harpoon_irc_channel");
    query.add(Database::OperationType::Join, "sender", "text", "harpoon_irc_sender");

    appQueue->sendEvent(eventSetup);*/

    auto fakeResult = std::make_shared<EventDatabaseResult>(event);
    fakeResult->setSuccess(true);
    getEventQueue()->sendEvent(fakeResult);
}

bool IrcBacklogService::setupTable_processResult(std::shared_ptr<IEvent> event) {
    auto result = event->as<EventDatabaseResult>();
    bool success = result->getSuccess();
    UUID originType = result->getEventOrigin()->getEventUuid();

    if (originType == EventInit::uuid) {
        if (success) {
            // request the last backlog entry id
            // TODO: select last id
            /* auto eventQueryLastId = std::make_shared<EventDatabaseQuery>(getEventQueue(), result->getEventOrigin());
            eventQueryLastId->add(Database::Query(Database::QueryType::LastId,
                                                  "harpoon_irc_backlog",
                                                  std::list<std::string>{"message_id"}));
            appQueue->sendEvent(eventQueryLastId); */

            auto fakeResult = std::make_shared<EventDatabaseResult>(result->getEventOrigin());
            fakeResult->setSuccess(true);
            fakeResult->addResult("1"); // LastID
            getEventQueue()->sendEvent(fakeResult);

            databaseInitialized = true;
        } else {
            std::cout << "Error setting up irc backlog service. Could not setup table. Service will be disabled" << std::endl;
            getEventQueue()->setEnabled(false);
            heldBackEvents.clear();
            appQueue->sendEvent(std::make_shared<EventIrcServiceInit>());
            return false;
        }
    }
    return true;
}

bool IrcBacklogService::setupTable_processId(std::shared_ptr<IEvent> event) {
    auto result = event->as<EventDatabaseResult>();
    bool success = result->getSuccess();
    UUID originType = result->getEventOrigin()->getEventUuid();

    if (originType == EventInit::uuid) {
        if (success) {
            auto& results = result->getResults();
            size_t lastId = 0;
            if (results.size() > 0)
                std::istringstream(results.front()) >> lastId;
            IdProvider::getInstance().setLowestId("irc_log", lastId);
            std::cout << "Database Last ID: " << lastId << std::endl;

            appQueue->sendEvent(std::make_shared<EventIrcServiceInit>());

            lastIdFetched = true;
        } else {
            std::cout << "Error setting up irc backlog service. Could not fetch last id. Service will be disabled" << std::endl;
            getEventQueue()->setEnabled(false);
            heldBackEvents.clear();
            appQueue->sendEvent(std::make_shared<EventIrcServiceInit>());
            return false;
        }
    }
    return true;
}

bool IrcBacklogService::processEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();

    if (eventType == EventQuit::uuid) {
        return false;
    }

    if (!databaseInitialized) {
        if (eventType == EventInit::uuid) {
            setupTable(event);
        } else if(eventType == EventDatabaseResult::uuid) {
            if (!setupTable_processResult(event))
                return false;
        } else {
            heldBackEvents.push_back(event);
        }
    } else if (!lastIdFetched) {
        if(eventType == EventDatabaseResult::uuid) {
            if (!setupTable_processId(event))
                return false;
            // process all held back events
            for (auto e : heldBackEvents)
                processEvent(e);
            heldBackEvents.clear();
        } else {
            heldBackEvents.push_back(event);
        }
    } else {
        auto loggable = event->as<IrcLoggable>();
        if (loggable != nullptr) {
            using namespace Query;

            if (eventType == EventIrcMessage::uuid) {
                auto message = event->as<EventIrcMessage>();

                std::vector<std::string> data {
                    std::to_string(loggable->getLogEntryId()),
                    convertTimestamp(message->getTimestamp()),
                    message->getMessage(),
                    message->getType() == MessageType::Notice ? "5" : "0",
                    "0",
                    message->getChannel(),
                    message->getFrom()
                };

                Insert stmt =
                    insert()
                    .into("harpoon_irc_backlog")
                    .format("message_id",
                            "time",
                            "message",
                            "type",
                            "flags",
                            "channel_name",
                            "sender_name")
                    .join("Harpoon_irc_channel", make_var("channel_id") == make_var("channel_ref"), "channel_name")
                    .join("Harpoon_irc_channel", make_var("sender_id") == make_var("sender_ref"), "sender_name")
                    .data(data.begin(), data.end());

                appQueue->sendEvent(std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt)));
            } else if (eventType == EventIrcRequestBacklog::uuid) {
                auto request = event->as<EventIrcRequestBacklog>();
                // TODO: select backlog
                /*auto eventFetch = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventFetch->add(Database::Query(Database::QueryType::Fetch,
                                                              "harpoon_irc_backlog",
                                                              std::list<std::string>{"message_id", "time", "message", "type", "flags", "channel"}));
                query.add(Database::OperationType::Join, "channel", "", "harpoon_irc_channel");
                query.add(Database::OperationType::CompareLower, "message_id", std::to_string(request->getFromId()+1));
                query.add(Database::OperationType::Limit, std::to_string(request->getCount()));

                appQueue->sendEvent(eventFetch);*/
            } else if (eventType == EventIrcAction::uuid) {
                auto action = event->as<EventIrcAction>();
                // TODO: insert action
                /*auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                               "harpoon_irc_backlog",
                                                               std::list<std::string>{"message_id", "time", "message", "type", "flags", "channel_ref", "sender_ref"}));
                query.add(Database::OperationType::Assign, std::to_string(loggable->getLogEntryId()));
                query.add(Database::OperationType::Assign, convertTimestamp(action->getTimestamp()));
                query.add(Database::OperationType::Assign, action->getMessage());
                query.add(Database::OperationType::Assign, "6");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "", "", "0");
                query.add(Database::OperationType::Assign, "", "", "1");
                query.add(Database::OperationType::Join, "channel", action->getChannel(), "harpoon_irc_channel");
                query.add(Database::OperationType::Join, "sender", action->getUsername(), "harpoon_irc_sender");

                appQueue->sendEvent(eventInsert);*/
            } else if (eventType == EventIrcJoined::uuid) {
                auto joined = event->as<EventIrcJoined>();
                // TODO: insert joined
                /*auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                               "harpoon_irc_backlog",
                                                               std::list<std::string>{"message_id", "time", "type", "flags", "channel_ref", "sender_ref"}));
                query.add(Database::OperationType::Assign, std::to_string(loggable->getLogEntryId()));
                query.add(Database::OperationType::Assign, convertTimestamp(joined->getTimestamp()));
                query.add(Database::OperationType::Assign, "1");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "", "", "0");
                query.add(Database::OperationType::Assign, "", "", "1");
                query.add(Database::OperationType::Join, "channel", joined->getChannel(), "harpoon_irc_channel");
                query.add(Database::OperationType::Join, "sender", joined->getUsername(), "harpoon_irc_sender");

                appQueue->sendEvent(eventInsert);*/
            } else if (eventType == EventIrcParted::uuid) {
                auto parted = event->as<EventIrcParted>();
                // TODO: insert parted
                /*auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                               "harpoon_irc_backlog",
                                                               std::list<std::string>{"message_id", "time", "type", "flags", "channel_ref", "sender_ref"}));
                query.add(Database::OperationType::Assign, std::to_string(loggable->getLogEntryId()));
                query.add(Database::OperationType::Assign, convertTimestamp(parted->getTimestamp()));
                query.add(Database::OperationType::Assign, "2");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "", "", "0");
                query.add(Database::OperationType::Assign, "", "", "1");
                query.add(Database::OperationType::Join, "channel", parted->getChannel(), "harpoon_irc_channel");
                query.add(Database::OperationType::Join, "sender", parted->getUsername(), "harpoon_irc_sender");

                appQueue->sendEvent(eventInsert);*/
            } else if (eventType == EventIrcQuit::uuid) {
                auto quit = event->as<EventIrcQuit>();
                // TODO: insert quit
                /*auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                               "harpoon_irc_backlog",
                                                               std::list<std::string>{"message_id", "time", "type", "flags", "channel_ref", "sender_ref"}));
                query.add(Database::OperationType::Assign, std::to_string(loggable->getLogEntryId()));
                query.add(Database::OperationType::Assign, convertTimestamp(quit->getTimestamp()));
                query.add(Database::OperationType::Assign, "3");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "", "", "0");
                query.add(Database::OperationType::Join, "sender", quit->getWho(), "harpoon_irc_sender");

                appQueue->sendEvent(eventInsert);*/
            } else if (eventType == EventIrcKicked::uuid) {
                auto kicked = event->as<EventIrcKicked>();
                // TODO: insert kick
                /*auto eventInsert = std::make_shared<EventDatabaseQuery>(getEventQueue(), event);
                auto& query = eventInsert->add(Database::Query(Database::QueryType::Insert,
                                                               "harpoon_irc_backlog",
                                                               std::list<std::string>{"message_id", "time", "message", "type", "flags", "channel_ref", "sender_ref"}));
                query.add(Database::OperationType::Assign, std::to_string(loggable->getLogEntryId()));
                query.add(Database::OperationType::Assign, convertTimestamp(kicked->getTimestamp()));
                query.add(Database::OperationType::Assign, kicked->getTarget());
                query.add(Database::OperationType::Assign, "4");
                query.add(Database::OperationType::Assign, "0");
                query.add(Database::OperationType::Assign, "", "", "0");
                query.add(Database::OperationType::Assign, "", "", "1");
                query.add(Database::OperationType::Join, "channel", kicked->getChannel(), "harpoon_irc_channel");
                query.add(Database::OperationType::Join, "sender", kicked->getUsername(), "harpoon_irc_sender");

                appQueue->sendEvent(eventInsert);*/
            }
        }
    }

    return true;
}
