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
using namespace Query;


enum class IrcDatabaseMessageType : int {
    Message = 0,
    Join = 1,
    Part = 2,
    Quit = 3,
    Kick = 4,
    Notice = 5,
    Action = 6
};


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
    Create stmtChannel = create("harpoon_irc_channel")
        .field("channel_id", FieldType::Id)
        .field("channel", FieldType::Text);
    Create stmtSender = create("harpoon_irc_sender")
        .field("sender_id", FieldType::Id)
        .field("sender", FieldType::Text);
    Create stmt = create("harpoon_irc_backlog")
        .field("message_id", FieldType::Id)
        .field("time", FieldType::Time)
        .field("message", FieldType::Text)
        .field("type", FieldType::Integer)
        .field("flags", FieldType::Integer)
        .field("channel_ref", FieldType::Integer)
        .field("sender_ref", FieldType::Integer);
    auto eventSetup = std::make_shared<EventDatabaseQuery>(getEventQueue(),
                                                           event,
                                                           std::move(stmtChannel),
                                                           std::move(stmtSender),
                                                           std::move(stmt));

    appQueue->sendEvent(eventSetup);

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
            Select stmt = select("message_id")
                .from("harpoon_irc_backlog")
                .order_by("message_id", "DESC")
                .limit(1);
            auto eventQueryLastId = std::make_shared<EventDatabaseQuery>(getEventQueue(),
                                                                    event,
                                                                    std::move(stmt));

            appQueue->sendEvent(eventQueryLastId);

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

void IrcBacklogService::writeBacklog(std::shared_ptr<IEvent> event,
                                     IrcLoggable* loggable,
                                     const std::string& message,
                                     IrcDatabaseMessageType type,
                                     const std::string& flags,
                                     const std::string& from,
                                     const std::string& channel) {
    std::vector<std::string> data {
        std::to_string(loggable->getLogEntryId()),
        convertTimestamp(event->getTimestamp()),
        message,
        std::to_string(static_cast<int>(type)),
        flags
    };

    Insert stmt =
        insert()
        .into("harpoon_irc_backlog")
        .format("message_id",
                "time",
                "message",
                "type",
                "flags")
        .join("Harpoon_irc_channel", "channel", channel)
        .join("Harpoon_irc_sender", "sender", from)
        .data(std::move(data));

    appQueue->sendEvent(std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt)));
}

bool IrcBacklogService::processEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();

    if (eventType == EventQuit::uuid)
        return false;

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
            if (eventType == EventIrcMessage::uuid) {
                auto message = event->as<EventIrcMessage>();
                writeBacklog(event,
                             loggable,
                             message->getMessage(),
                             message->getType() == MessageType::Message
                             ? IrcDatabaseMessageType::Message
                             : IrcDatabaseMessageType::Notice,
                             "0",
                             message->getFrom(),
                             message->getChannel());
            } else if (eventType == EventIrcRequestBacklog::uuid) {
                auto request = event->as<EventIrcRequestBacklog>();

                Select stmt = select("message_id", "time", "message", "type", "flags", "channel", "sender")
                    .from("harpoon_irc_backlog")
                    .join("harpoon_irc_channel", "channel")
                    .join("harpoon_irc_sender", "sender")
                    .order_by("message_id", "DESC")
                    .limit(5);
                auto eventFetch = std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt));

                appQueue->sendEvent(eventFetch);
            } else if (eventType == EventIrcAction::uuid) {
                auto action = event->as<EventIrcAction>();
                writeBacklog(event,
                             loggable,
                             action->getMessage(),
                             IrcDatabaseMessageType::Action,
                             "0",
                             action->getUsername(),
                             action->getChannel());
            } else if (eventType == EventIrcJoined::uuid) {
                auto joined = event->as<EventIrcJoined>();
                writeBacklog(event,
                             loggable,
                             "",
                             IrcDatabaseMessageType::Join,
                             "0",
                             joined->getUsername(),
                             joined->getChannel());
            } else if (eventType == EventIrcParted::uuid) {
                auto parted = event->as<EventIrcParted>();
                writeBacklog(event,
                             loggable,
                             "",
                             IrcDatabaseMessageType::Part,
                             "0",
                             parted->getUsername(),
                             parted->getChannel());
            } else if (eventType == EventIrcQuit::uuid) {
                auto quit = event->as<EventIrcQuit>();
                writeBacklog(event,
                             loggable,
                             "",
                             IrcDatabaseMessageType::Quit,
                             "0",
                             quit->getWho(),
                             "");
            } else if (eventType == EventIrcKicked::uuid) {
                auto kicked = event->as<EventIrcKicked>();
                writeBacklog(event,
                             loggable,
                             kicked->getReason(),
                             IrcDatabaseMessageType::Kick,
                             "0",
                             kicked->getUsername(),
                             kicked->getChannel());
            }
        }
    }

    return true;
}
