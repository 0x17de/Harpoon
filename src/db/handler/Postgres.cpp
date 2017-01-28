#include "Postgres.hpp"
#include "db/query/Database_Query.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "utils/Ini.hpp"

#include <iostream>
#include <limits>
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

        static std::string fieldTypeName(Query::FieldType type);

        void query_createTable(Query::QueryCreate_Store* store, EventDatabaseResult* result);
        void query_insert(Query::QueryInsert_Store* store, EventDatabaseResult* result);
        void query_select(Query::QuerySelect_Store* store, EventDatabaseResult* result);
        void query_delete(Query::QueryDelete_Store* store, EventDatabaseResult* result);

        static Query::TraverseCallbacks getTraverseCallbacks(stringstream& ss, size_t& filterDataIndex);

        std::list<std::shared_ptr<IEvent>> heldBackQueries;

        friend Postgres;
    };

    Query::TraverseCallbacks Postgres_Impl::getTraverseCallbacks(stringstream& ss, size_t& filterDataIndex) {
        return {
            // up
            [&ss]{ss << '(';},
            // down
            [&ss]{ss << ')';},
            // variable
            [&ss](const std::string& name){ss << name;},
            // contant
            [&ss, &filterDataIndex](const std::string& name){ss << ":data" << filterDataIndex++; },
            // operation
            [&ss](Query::Op op){
                switch(op) {
                case Query::Op::EQ: ss << " = "; break;
                case Query::Op::NEQ: ss << " != "; break;
                case Query::Op::GT: ss << " > "; break;
                case Query::Op::LT: ss << " < "; break;
                case Query::Op::AND: ss << " && "; break;
                case Query::Op::OR: ss << " || "; break;
                }
            }
        };
    }

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

    std::string Postgres_Impl::fieldTypeName(Query::FieldType type) {
        using namespace Query;

        switch(type) {
        case FieldType::Id:
            return "serial primary key";
        case FieldType::Time:
            return "timestamp";
        case FieldType::Integer:
            return "integer";
        case FieldType::Text:
            return "text";
        case FieldType::Bool:
            return "boolean";
        }
        return "INVALID";
    }

    void Postgres_Impl::query_createTable(Query::QueryCreate_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        size_t index = 0;
        stringstream ss;
        ss << "CREATE TABLE IF NOT EXISTS " << store->name << " (";
        for (auto& field : store->fields) {
            ss << field.name << " " << fieldTypeName(field.type);
            ++index;
            if (index < store->fields.size())
                ss << ", ";
        }
        ss << ")" << endl;

        sqlSession->once << ss.str();

        result->setSuccess(true);
    }

    void Postgres_Impl::query_insert(Query::QueryInsert_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        std::vector<size_t> joinIds(store->on.size());

        { // SELECT(JOIN)
            size_t joinIndex = 0;
            for (auto& join : store->on) {
                stringstream ss;
                ss << "SELECT " << join.field << "_id FROM "
                   << join.table
                   << " WHERE " << join.field << " = :data" << joinIndex << " LIMIT 1";

                soci::statement st = (sqlSession->prepare << ss.str(), soci::use(join.on), soci::into(joinIds[joinIndex])); // cast
                st.execute();
                st.fetch(); // will write joinId

                ++joinIndex;
            }
        }

        { // JOIN
            size_t joinIndex = 0;
            for (auto& join : store->on) {
                if (joinIds[joinIndex] != 0) continue; // already found in database

                stringstream ss;
                ss << "INSERT INTO "
                   << join.table
                   << " (" << join.field << ") VALUES (:data)";

                    sqlSession->once << ss.str(), soci::use(join.on);
                sqlSession->once << "SELECT CURRVAL('" << join.table << "_" << join.field << "_id_seq')", soci::into(joinIds[joinIndex]);

                ++joinIndex;
            }
        }

        { // INSERT
            stringstream ss;

            ss << "INSERT INTO " << store->into << " (";
            size_t index = 0;
            for (auto& s : store->format) {
                ss << s;
                ++index;
                if (index < store->format.size())
                    ss << ", ";
            }
            for (auto& join : store->on)
                ss << ", " << join.field << "_ref";
            ss << ") VALUES ";

            size_t dataIndex = 0;
            decltype(store->data.cend()) end;
            for (auto it = store->data.cbegin(); it != store->data.cend(); it = end) {
                end = std::next(it, store->format.size());

                ss << "(";

                size_t subIndex = 0;
                for (auto q = it; q != end; ++q) {
                    ss << ":data" << dataIndex << '_' << subIndex;
                    ++subIndex;
                    if (subIndex < store->format.size())
                        ss << ", ";
                }
                for (size_t i : joinIds)
                    ss << ", " << i; // is the join id (number). can not be exploited

                ss << ")";

                ++dataIndex;
                if (end != store->data.end())
                    ss << ", ";
            }
            ss << ";";

            {
                auto query = sqlSession->once << ss.str();
                for (auto& s : store->data)
                    query, soci::use(s);
            }
        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_select(Query::QuerySelect_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        stringstream ss;
        ss << "SELECT ";
        size_t whatIndex = 0;
        for (auto& s : store->what) {
            ss << s;
            ++whatIndex;
            if (whatIndex < store->what.size())
                ss << ", ";
        }
        ss << " FROM " << store->from;

        // TODO: join

        if (store->filter) {
            size_t filterDataIndex = 0;
            ss << " WHERE ";
            store->filter->traverse(getTraverseCallbacks(ss, filterDataIndex));
        }

        if (store->order.size() > 0) {
            ss << " ORDER BY";
            for (auto& order : store->order)
                ss << " " << order.first << " " << order.second;
        }

        if (store->limit != std::numeric_limits<size_t>::max())
            ss << " LIMIT " << store->limit;

        {
            auto query = sqlSession->prepare << ss.str();
            if (store->filter) {
                store->filter->traverse(TraverseCallbacks{
                        []{},
                        []{},
                        [](const std::string& name){ },
                        [&query](const std::string& name){ query, soci::use(name); },
                        [](Op op) {}
                    });
            }

            std::list<std::string> temp(store->what.size());
            for (auto& s : temp)
                query, soci::into(s);

            soci::statement st = query; // cast
            st.execute();
            while (st.fetch()) {
                for (auto& s : temp)
                    result->addResult(s);
            }

        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_delete(Query::QueryDelete_Store* store, EventDatabaseResult* result) {
        using namespace Query;

        stringstream ss;
        ss << "DELETE FROM " << store->from;

        // TODO: join

        if (store->filter) {
            size_t filterDataIndex = 0;
            ss << " WHERE ";
            store->filter->traverse(getTraverseCallbacks(ss, filterDataIndex));
        }

        if (store->limit != std::numeric_limits<size_t>::max())
            ss << " LIMIT " << store->limit;

        {
            auto query = sqlSession->once << ss.str();
            if (store->filter) {
                store->filter->traverse(TraverseCallbacks{
                        []{},
                        []{},
                        [](const std::string& name){ },
                        [&query](const std::string& name){ query, soci::use(name); },
                        [](Op op) {}
                    });
            }
        }

        result->setSuccess(true);
    }

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
                if (select) {
                    query_select(select, result.get());
                } else {
                    auto erase = dynamic_cast<Query::QueryDelete_Store*>(ptr);
                    if (erase) {
                        query_delete(erase, result.get());
                    } else {
                        auto create = dynamic_cast<Query::QueryCreate_Store*>(ptr);
                        if (create) {
                            query_createTable(create, result.get());
                        }
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
