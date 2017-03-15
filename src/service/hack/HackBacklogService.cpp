#include "HackBacklogService.hpp"
#include "HackDatabaseMessageType.hpp"
#include "db/query/Database_Query.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/hack/EventHackServiceInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "event/IUserEvent.hpp"
#include "event/hack/HackLoggable.hpp"
// ...
#include "utils/IdProvider.hpp"
#include "utils/ModuleProvider.hpp"

#include <limits>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <sstream>


PROVIDE_EVENTLOOP_MODULE("hack_backlog", "default", HackBacklogService)
using namespace Query;


HackBacklogService::HackBacklogService(EventQueue* appQueue)
    : EventLoop({
                    EventDatabaseQuery::uuid
                  , EventDatabaseResult::uuid
               // , EventHackRequestBacklog::uuid // TODO
                  , EventQuit::uuid
                },
                {
                    &EventGuard<HackLoggable>
                })
    , appQueue{appQueue}
    , databaseInitialized{false}
    , lastIdFetched{false}
{
}

HackBacklogService::~HackBacklogService() {
}

bool HackBacklogService::onEvent(std::shared_ptr<IEvent> event) {
    return processEvent(event);
}

std::string HackBacklogService::convertTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp) {
    std::time_t t = std::chrono::system_clock::to_time_t(timestamp);
    return std::ctime(&t);
}

void HackBacklogService::setupTable(std::shared_ptr<IEvent> event) {
    Create stmtChannel = create("harpoon_hack_channel")
        .field("channel_id", FieldType::Id)
        .field("channel", FieldType::Text);
    Create stmtSender = create("harpoon_hack_sender")
        .field("sender_id", FieldType::Id)
        .field("sender", FieldType::Text)
        .field("trip", FieldType::Text);
    Create stmt = create("harpoon_hack_backlog")
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

bool HackBacklogService::setupTable_processResult(std::shared_ptr<IEvent> event) {
    auto result = event->as<EventDatabaseResult>();
    bool success = result->getSuccess();
    UUID originType = result->getEventOrigin()->getEventUuid();

    if (originType == EventInit::uuid) {
        if (success) {
            // request the last backlog entry id
            Select stmt = select("message_id")
                .from("harpoon_hack_backlog")
                .order_by("message_id", "DESC")
                .limit(1);
            auto eventQueryLastId = std::make_shared<EventDatabaseQuery>(getEventQueue(),
                                                                         result->getEventOrigin(),
                                                                         std::move(stmt));

            appQueue->sendEvent(eventQueryLastId);

            databaseInitialized = true;
        } else {
            std::cout << "Error setting up hack backlog service. Could not setup table. Service will be disabled" << std::endl;
            getEventQueue()->setEnabled(false);
            heldBackEvents.clear();
            appQueue->sendEvent(std::make_shared<EventHackServiceInit>());
            return false;
        }
    }
    return true;
}

bool HackBacklogService::setupTable_processId(std::shared_ptr<IEvent> event) {
    auto result = event->as<EventDatabaseResult>();
    bool success = result->getSuccess();
    UUID originType = result->getEventOrigin()->getEventUuid();

    if (originType == EventInit::uuid) {
        if (success) {
            auto& results = result->getResults();
            size_t lastId = 0;
            if (results.size() > 0)
                std::istringstream(results.front()) >> lastId;
            IdProvider::getInstance().setLowestId("hack_log", lastId);
            std::cout << "Database Last ID: " << lastId << std::endl;

            appQueue->sendEvent(std::make_shared<EventHackServiceInit>());

            lastIdFetched = true;
        } else {
            std::cout << "Error setting up hack backlog service. Could not fetch last id. Service will be disabled" << std::endl;
            getEventQueue()->setEnabled(false);
            heldBackEvents.clear();
            appQueue->sendEvent(std::make_shared<EventHackServiceInit>());
            return false;
        }
    }
    return true;
}

void HackBacklogService::writeBacklog(std::shared_ptr<IUserEvent> event,
                                     HackLoggable* loggable,
                                     const std::string& message,
                                     HackDatabaseMessageType type,
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
        .into("harpoon_hack_backlog")
        .format("message_id",
                "user_id",
                "time",
                "message",
                "type",
                "flags")
        .join("Harpoon_hack_channel", "channel", channel)
        .join("Harpoon_hack_sender", "sender", from)
        .data(std::move(data));

    appQueue->sendEvent(std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt)));
}

bool HackBacklogService::processEvent(std::shared_ptr<IEvent> event) {
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
        auto loggable = event->as<HackLoggable>();
        if (loggable == nullptr) {
            /*
            switch(eventType) {
            case EventHackRequestBacklog::uuid:
                {
                    auto request = event->as<EventHackRequestBacklog>();

                    Select stmt = select("channel_id")
                        .from("harpoon_hack_channel")
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
                        case EventHackRequestBacklog::uuid: {
                            auto request = resultOrigin->as<EventHackRequestBacklog>();

                            auto& results = result->getResults();
                            if (results.size() == 0) {
                                // no results -> send empty response
                                appQueue->sendEvent(std::make_shared<EventHackBacklogResponse>(request->getUserId(),
                                                                                              request->getServerId(),
                                                                                              request->getChannelName(),
                                                                                              std::list<MessageData>()));
                            } else {
                                // we got the channel_ref id
                                std::string channelRefId = results.front();

                                auto req = make_var("channel_ref") == make_constant(channelRefId)
                                    && make_var("user_id") == make_constant(std::to_string(request->getUserId()));
                                auto fromId = request->getFromId();
                                bool noFromId = fromId == std::numeric_limits<size_t>::max();

                                Select stmt = select("message_id", "time", "message", "type", "flags", "sender")
                                    .from("harpoon_hack_backlog")
                                    .join("harpoon_hack_sender", "sender")
                                    .where(noFromId
                                           ? std::move(req)
                                           : std::move(std::move(req) && make_var("message_id") < make_constant(std::to_string(fromId))))
                                    .order_by("message_id", "DESC")
                                    .limit(100); // amount of log lines fetched
                                auto eventFetch = std::make_shared<EventDatabaseQuery>(getEventQueue(), event, std::move(stmt));

                                appQueue->sendEvent(eventFetch);
                            }
                            break;
                        } // case EventHackBacklogResponse::uuid
                        case EventDatabaseResult::uuid: {
                            auto realOrigin = resultOrigin->as<EventDatabaseResult>()->getEventOrigin();
                            if (realOrigin->getEventUuid() == EventHackRequestBacklog::uuid) {
                                auto request = realOrigin->as<EventHackRequestBacklog>();

                                std::list<MessageData> data;

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
                                                      static_cast<HackDatabaseMessageType>(type),
                                                      flags,
                                                      sender);
                                }

                                auto response = std::make_shared<EventHackBacklogResponse>(request->getUserId(),
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
            */
        } else {
            switch(eventType) {
                // TODO: EventHackMessage
            }
        }
    }

    return true;
}
