#include "Postgres.hpp"
#include "db/query/Database_Query.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "utils/Ini.hpp"

#include <iostream>
#include <type_traits>
#include <soci/soci.h>

using namespace std;


namespace Database {

    PROVIDE_MODULE("database", "postgres", Postgres);


    struct Postgres_Impl {
        bool connectionFailed;
        EventQueue* appQueue;
        shared_ptr<soci::session> sqlSession;
        explicit Postgres_Impl(EventQueue* appQueue) : connectionFailed{false}, appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        void handleQuery(std::shared_ptr<IEvent> event);

        void query_createTable(Query::QueryCreate_Store* store, EventDatabaseResult* result);
        void query_insert(Query::QueryInsert_Store* store, EventDatabaseResult* result);
        void query_select(Query::QuerySelect_Store* store, EventDatabaseResult* result);
        //void query_delete(const std::unique_ptr<Query::QueryDelete_Store>& query, EventDatabaseResult* result);

        std::list<std::shared_ptr<IEvent>> heldBackQueries;

        friend Postgres;
    };

    Postgres::Postgres(EventQueue* appQueue)
        : EventLoop{
            {},
            {
                &EventGuard<IDatabaseEvent>
            }
        }
        , impl{make_shared<Postgres_Impl>(appQueue)}
    {
    }

    Postgres::~Postgres() {
    }

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    void Postgres_Impl::query_createTable(Query::QueryCreate_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        size_t index = 0;
        stringstream ss;
        ss << "CREATE TABLE IF NOT EXISTS " << store->name << " (";
        for (auto& field : store->fields) {
            switch(field.type) {
            case FieldType::Id:
                ss << field.name << " serial" << endl;
                break;
            case FieldType::Time:
                ss << field.name << " timestamp" << endl;
                break;
            case FieldType::Integer:
                ss << field.name << " integer" << endl;
                break;
            case FieldType::Text:
                ss << field.name << " text" << endl;
                break;
            case FieldType::Bool:
                ss << field.name << " boolean" << endl;
                break;
            }

            ++index;
            if (index < store->fields.size())
                cout << ", ";
        }
        ss << ")" << endl;

        cout << ss.str() << endl;
        sqlSession->once << ss.str();

        result->setSuccess(true);
    }

    void Postgres_Impl::query_insert(Query::QueryInsert_Store* store, EventDatabaseResult* result) {
        using namespace Query;
        stringstream ss;

        ss << "BEGIN;";

        // TODO: insert data required for join

        ss << "INSERT INTO " << store->into << " (";
        size_t index = 0;
        for (auto& s : store->format) {
            cout << s;
            ++index;
            if (index < store->format.size())
                cout << ", ";
        }
        ss << ") VALUES ";

        size_t dataIndex = 0;
        decltype(store->data.cend()) end;
        for (auto it = store->data.cbegin(); it != store->data.cend(); it = end + store->on.size()) {
            end = it + store->format.size();

            ss << "(";

            size_t subIndex = 0;
            for (auto q = it; q != end; ++q) {
                ss << *q;
                ++subIndex;
                if (subIndex < store->format.size())
                    ss << ", ";
            }

            ss << ") ";
            ++dataIndex;
        }

        ss << ";";
        ss << "COMMIT;";
        sqlSession->once << ss.str();

        result->setSuccess(true);
    }

    void Postgres_Impl::query_select(Query::QuerySelect_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        stringstream ss;
        ss << "SELECT ";
        size_t whatIndex = 0;
        for (auto& s : store->what) {
            cout << s;
            ++whatIndex;
            if (whatIndex < store->what.size())
                cout << ", ";
        }
        ss << " FROM " << store->from;

        // TODO: join

        if (store->filter) {
            size_t filterDataIndex = 0;
            ss << "WHERE ";
            store->filter->traverse(TraverseCallbacks{
                    // up
                    []{cout << '(';},
                    // down
                    []{cout << ')';},
                    // variable
                    [&ss](const std::string& name){ss << '`' << name << '`';},
                    // contant
                    [&ss, &filterDataIndex](const std::string& name){ss << ":data" << filterDataIndex++; },
                    // operation
                    [&ss](Op op){
                        switch(op) {
                        case Query::Op::EQ: ss << " == "; break;
                        case Query::Op::NEQ: ss << " != "; break;
                        case Query::Op::GT: ss << " > "; break;
                        case Query::Op::LT: ss << " < "; break;
                        case Query::Op::AND: ss << " && "; break;
                        case Query::Op::OR: ss << " || "; break;
                        }
                    }
                });
        }

        {
            auto query = sqlSession->once << ss.str();
            store->filter->traverse(TraverseCallbacks{
                    []{},
                    []{},
                    [](const std::string& name){ },
                    [&query](const std::string& name){ query, name; },
                    [](Op op) {}
                });
        }

        result->setSuccess(true);
    }

    /*
    void Postgres_Impl::query_delete(const std::unique_ptr<Query::QueryBase>& query, EventDatabaseResult* result) {
        // TODO: delete

        result->setSuccess(true);
    }
    */

    void Postgres_Impl::handleQuery(std::shared_ptr<IEvent> event) {
        EventDatabaseQuery* query = event->as<EventDatabaseQuery>();
        auto result = make_shared<EventDatabaseResult>(query->getEventOrigin());
        auto db = event->as<EventDatabaseQuery>();

        if (connectionFailed) {
            query->getTarget()->sendEvent(result); // send 'failed' status
            return;
        }

        for (const auto& subQuery : db->getQueries()) {
            auto ptr = subQuery.get();
            auto insert = dynamic_cast<Query::QueryInsert_Store*>(ptr);
            if (insert) {
                query_insert(insert, result.get());
            } else {
                auto select = dynamic_cast<Query::QuerySelect_Store*>(ptr);
                if (insert) {
                    query_select(select, result.get());
                } else {
                    auto create = dynamic_cast<Query::QueryCreate_Store*>(ptr);
                    if (create) {
                        query_createTable(create, result.get());
                    }
                }
            }
        }

        query->getTarget()->sendEvent(result);
    }

    bool Postgres_Impl::onEvent(std::shared_ptr<IEvent> event) {
        UUID eventType = event->getEventUuid();
        if (eventType == EventQuit::uuid) {
            return false;
        } else if (eventType == EventDatabaseQuery::uuid) {
            if (sqlSession || connectionFailed) {
                handleQuery(event);
            } else {
                heldBackQueries.push_back(event);
            }
        } else if (eventType == EventInit::uuid) {
            using namespace soci;

            string host,
                port,
                username,
                password,
                database;

            Ini dbIni("config/postgres.ini");
            auto& auth = dbIni.expectCategory("auth");
            dbIni.getEntry(auth, "host", host);
            dbIni.getEntry(auth, "port", port);
            dbIni.getEntry(auth, "username", username);
            dbIni.getEntry(auth, "password", password);
            dbIni.getEntry(auth, "database", database);

#pragma message "check if values are 'evil'"
            stringstream login;
            login << "postgresql://";
            if (host.size() > 0)
                login << "host=" << host << " ";
            if (port.size() > 0)
                login << "port=" << port << " ";
            login << "dbname=" << database << " "
                  << "user=" << username << " "
                  << "password=" << password;

            try {
                sqlSession = make_shared<soci::session>(login.str());
            } catch(soci_error& e) {
                connectionFailed = true;
                cout << "Could not connect to database server. Reason: " << endl << e.what() << endl << endl;
            }

            for (auto query : heldBackQueries)
                handleQuery(query);
            heldBackQueries.clear();
        }
        return true;
    }

}
