#include "IrcBacklogService.hpp"
#include "IrcDatabaseMessageType.hpp"
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
#include "event/irc/EventIrcBacklogResponse.hpp"
#include "utils/IdProvider.hpp"
#include "utils/ModuleProvider.hpp"

#include <limits>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>


PROVIDE_MODULE("irc_backlog", "default", IrcBacklogService);
using namespace Query;


IrcBacklogService::IrcBacklogService(EventQueue* appQueue)
    : EventLoop({
                    EventDatabaseQuery::uuid
                  , EventDatabaseResult::uuid
                  , EventIrcRequestBacklog::uuid
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
        .field("user_id", FieldType::Integer)
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
                                                                         result->getEventOrigin(),
                                                                         std::move(stmt));

            appQueue->sendEvent(eventQueryLastId);

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

void IrcBacklogService::writeBacklog(std::shared_ptr<IUserEvent> event,
                                     IrcLoggable* loggable,
                                     const std::string& message,
                                     IrcDatabaseMessageType type,
                                     const std::string& flags,
                                     const std::string& from,
                                     const std::string& channel) {
    std::vector<std::string> data {
        std::to_string(loggable->getLogEntryId()),
        std::to_string(event->getUserId()),
        convertTimestamp(event->getTimestamp()),
        message,
        std::to_string(static_cast<int>(type)),
        flags
    };

    Insert stmt =
        insert()
        .into("harpoon_irc_backlog")
        .format("message_id",
                "user_id",
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
        if (loggable == nullptr) {
            switch(eventType) {
            case EventIrcRequestBacklog::uuid:
                {
                    auto request = event->as<EventIrcRequestBacklog>();

                    Select stmt = select("channel_id")
                        .from("harpoon_irc_channel")
                        .where(make_var("channel") == make_constant(request->getChannelName()))
                        .limit(1);
                    auto eventFetch = std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt));

                    appQueue->sendEvent(eventFetch);
                    break;
                }
            case EventDatabaseResult::uuid: // RESULT
                {
                    auto result = event->as<EventDatabaseResult>();
                    if (result->getSuccess()) {
                        auto resultOrigin = result->getEventOrigin();
                        switch (resultOrigin->getEventUuid()) {
                        case EventIrcRequestBacklog::uuid: {
                            auto request = resultOrigin->as<EventIrcRequestBacklog>();

                            auto& results = result->getResults();
                            if (results.size() == 0) {
                                // no results -> send empty response
                                appQueue->sendEvent(std::make_shared<EventIrcBacklogResponse>(request->getUserId(),
                                                                                              request->getServerId(),
                                                                                              request->getChannelName(),
                                                                                              std::list<IrcMessageData>()));
                            } else {
                                // we got the channel_ref id
                                std::string channelRefId = results.front();

                                auto req = make_var("channel_ref") == make_constant(channelRefId)
                                    && make_var("user_id") == make_constant(std::to_string(request->getUserId()));
                                auto fromId = request->getFromId();
                                bool noFromId = fromId == std::numeric_limits<size_t>::max();

                                Select stmt = select("message_id", "time", "message", "type", "flags", "sender")
                                    .from("harpoon_irc_backlog")
                                    .join("harpoon_irc_sender", "sender")
                                    .where(noFromId
                                           ? std::move(req)
                                           : std::move(std::move(req) && make_var("message_id") < make_constant(std::to_string(fromId))))
                                    .order_by("message_id", "DESC")
                                    .limit(100); // amount of log lines fetched
                                auto eventFetch = std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt));

                                appQueue->sendEvent(eventFetch);
                            }
                            break;
                        } // case EventIrcBacklogResponse::uuid
                        case EventDatabaseResult::uuid: {
                            auto realOrigin = resultOrigin->as<EventDatabaseResult>()->getEventOrigin();
                            if (realOrigin->getEventUuid() == EventIrcRequestBacklog::uuid) {
                                auto request = realOrigin->as<EventIrcRequestBacklog>();

                                std::list<IrcMessageData> data;

                                auto it = result->getResults().begin();
                                auto end = result->getResults().end();
                                while (it != end) {
                                    size_t messageId;
                                    size_t type;
                                    size_t flags;

                                    std::istringstream(*it++) >> messageId;

                                    std::tm t = {};
                                    std::istringstream ss(*it++); // read in time
                                    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
                                    if (ss.fail()) {
                                        std::cout << "Could not parse date from backlog response" << std::endl;
                                    }
                                    std::time_t time = std::mktime(&t);

                                    std::string message = *it++;
                                    std::istringstream(*it++) >> type;
                                    std::istringstream(*it++) >> flags;
                                    std::string sender = *it++;

                                    data.emplace_back(messageId,
                                                      time,
                                                      message,
                                                      static_cast<IrcDatabaseMessageType>(type),
                                                      flags,
                                                      sender);
                                }

                                auto response = std::make_shared<EventIrcBacklogResponse>(request->getUserId(),
                                                                                          request->getServerId(),
                                                                                          request->getChannelName(),
                                                                                          std::move(data));

                                appQueue->sendEvent(response);
                            }
                            break;
                        } // case EventDatabaseResult::uuid
                        } // switch (resultOrigin->getEventUuid())
                    }
                    break;
                }
            }
        } else {
            switch(eventType) {
            case EventIrcMessage::uuid:
                {
                    auto message = event->as<EventIrcMessage>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 message->getMessage(),
                                 message->getType() == IrcMessageType::Message
                                 ? IrcDatabaseMessageType::Message
                                 : IrcDatabaseMessageType::Notice,
                                 "0",
                                 message->getFrom(),
                                 message->getChannel());
                    break;
                }
            case EventIrcAction::uuid:
                {
                    auto action = event->as<EventIrcAction>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 action->getMessage(),
                                 IrcDatabaseMessageType::Action,
                                 "0",
                                 action->getUsername(),
                                 action->getChannel());
                    break;
                }
            case EventIrcJoined::uuid:
                {
                    auto joined = event->as<EventIrcJoined>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 "",
                                 IrcDatabaseMessageType::Join,
                                 "0",
                                 joined->getUsername(),
                                 joined->getChannel());
                    break;
                }
            case EventIrcParted::uuid:
                {
                    auto parted = event->as<EventIrcParted>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 "",
                                 IrcDatabaseMessageType::Part,
                                 "0",
                                 parted->getUsername(),
                                 parted->getChannel());
                    break;
                }
            case EventIrcQuit::uuid:
                {
                    auto quit = event->as<EventIrcQuit>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 "",
                                 IrcDatabaseMessageType::Quit,
                                 "0",
                                 quit->getWho(),
                                 "");
                    break;
                }
            case EventIrcKicked::uuid:
                {
                    auto kicked = event->as<EventIrcKicked>();
                    writeBacklog(std::static_pointer_cast<IUserEvent>(event),
                                 loggable,
                                 kicked->getReason(),
                                 IrcDatabaseMessageType::Kick,
                                 "0",
                                 kicked->getUsername(),
                                 kicked->getChannel());
                    break;
                }
            }
        }
    }

    return true;
}
