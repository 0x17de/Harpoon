#include "Postgres.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "utils/Ini.hpp"

#include <iostream>
#include <soci/soci.h>

using namespace std;


namespace Database {

    PROVIDE_MODULE("database", "postgres", Postgres);


    struct Postgres_Impl {
        EventQueue* appQueue;
        shared_ptr<soci::session> sqlSession;
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        void handleQuery(std::shared_ptr<IEvent> event);

        void query_setupDatabase(const Database::Query& query);
        void query_insert(const Database::Query& query);
        void query_fetch(const Database::Query& query);
        void query_delete(const Database::Query& query);

        void query_scanOperations(const Database::Query& query,
                                  std::list<Database::Operation const *>& join,
                                  Database::Operation const *& where,
                                  Database::Operation const *& limit);
        void query_handleWhere(soci::details::once_type& once, const Database::Query& query);
        void query_handleWhere(soci::details::once_type& once, const Database::Operation& op, size_t& index);

        static const std::map<std::string, std::string> typeMap;
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

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    const map<string, string> Postgres_Impl::typeMap {
        {"id", "serial"},
        {"timestamp", "timestamp"},
        {"int", "integer"},
        {"text", "text"},
        {"bool", "boolean"}
    };

    void Postgres_Impl::query_setupDatabase(const Database::Query& query) {
        auto once(sqlSession->once);
        once << "CREATE TABLE IF NOT EXISTS " << query.getTable() << " "
             << "(";
        bool first = true;
        for (auto& op : query.getOperations()) {
            if (op.getOperation() != Database::OperationType::Assign)
                continue;
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << typeMap.at(op.getLeft()) << " " << op.getRight();
        }
        once << ")";
    }

    void Postgres_Impl::query_insert(const Database::Query& query) {
        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        std::vector<size_t> ids;
        size_t joinIndex = 0;
        for (auto& op : join) {
            ids.push_back(0);
            sqlSession->once << "SELECT id FROM "
                             << op->getExtra() << " WHERE "
                             << op->getLeft() << " = :sel" << joinIndex, soci::into(ids.back()), soci::use(op->getRight());
            if (ids.back() == 0) { // not found
                sqlSession->once << "INSERT INTO "
                                 << op->getExtra() << "(" << op->getLeft() << ")"
                                 << " VALUES (:ins" << joinIndex << ")", soci::use(op->getRight());
            }
            ++joinIndex;
        }

        auto once(sqlSession->once);
        once << "INSERT INTO " << query.getTable() << " "
             << "(";
        bool first = true;
        for (auto& op : query.getOperations()) {
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << op.getLeft();
        }
        once << ") VALUES (";
        size_t index = 0;
        for (auto& op : query.getOperations()) {
            string right = op.getRight();
            if (op.getExtra().size() > 0) {
                size_t idsIndex;
                istringstream(op.getExtra()) >> idsIndex;
                right = ids.at(idsIndex);
            }
            if (op.getOperation() != Database::OperationType::Assign)
                continue;
            if (index == 0)
                once << ", ";
            once << ":op" << index;
            once, soci::use(right);
            ++index;
        }
        once << ")";
    }

    void Postgres_Impl::query_fetch(const Database::Query& query) {
#warning Postgres QueryType::Fetch stub
        auto once(sqlSession->once);
        once << "SELECT ";
        bool first = true;
        for (auto& column : query.getColumns()) {
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << column;
        }
        once << " FROM " << query.getTable();

        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        if (where) {
            size_t index;
            query_handleWhere(once, *where, index);
        }
    }

    void Postgres_Impl::query_delete(const Database::Query& query) {
#warning Postgres QueryType::Delete stub
        auto once(sqlSession->once);
        once << "DELETE ";
        bool first = true;
        for (auto& column : query.getColumns()) {
            if (first) {
                first = false;
            } else {
                once << ", ";
            }
            once << column;
        }
        once << " FROM " << query.getTable();

        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        if (where) {
            size_t index;
            query_handleWhere(once, *where, index);
        }
    }

    void Postgres_Impl::query_scanOperations(const Database::Query& query,
                                             std::list<Database::Operation const *>& join,
                                             Database::Operation const *& where,
                                             Database::Operation const *& limit) {
        for (auto& op : query.getOperations()) {
            switch (op.getOperation()) {
            case Database::OperationType::CompareAnd:
            case Database::OperationType::CompareOr:
                if (where == 0)
                    where = &op;
                break;
            case Database::OperationType::Join:
                join.push_back(&op);
                break;
            case Database::OperationType::Limit:
                if (limit == 0)
                    limit = &op;
                break;
            }
        }
    }

    void Postgres_Impl::query_handleWhere(soci::details::once_type& once, const Database::Operation& op, size_t& index) {
        if (op.getOperation() == Database::OperationType::Assign) {
            once << op.getLeft() << " = " << ":where" << index;
            once, op.getRight();
            return;
        }

        auto& subOps = op.getSuboperations();
        if (subOps.size() == 0) return;

        bool first = true;
        once << "(";
        for (auto& subOp : subOps) {
            if (first) {
                first = false;
            } else {
                if (op.getOperation() == Database::OperationType::CompareOr) {
                    once << " OR ";
                } else {
                    once << " AND ";
                }
            }
            query_handleWhere(once, subOp, index);
        }
        once << ")";
    }

    void Postgres_Impl::handleQuery(std::shared_ptr<IEvent> event) {
        auto db = event->as<EventDatabaseQuery>();
        for (const auto& query : db->getQueries()) {
            switch (query.getType()) {
            case Database::QueryType::SetupTable:
                query_setupDatabase(query);
                break;
            case Database::QueryType::Fetch:
                query_fetch(query);
                break;
            case Database::QueryType::Insert:
                query_insert(query);
                break;
            case Database::QueryType::Delete:
                query_delete(query);
                break;
            }
        }
    }

    bool Postgres_Impl::onEvent(std::shared_ptr<IEvent> event) {
        UUID eventType = event->getEventUuid();
        if (eventType == EventQuit::uuid) {
            return false;
        } else if (eventType == EventDatabaseQuery::uuid) {
            if (sqlSession) {
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

#warning check if values are 'evil'
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
                cout << "Could not connect to database server. Reason: " << endl << e.what() << endl << endl;
                return false;
            }

            for (auto query : heldBackQueries)
                handleQuery(query);
            heldBackQueries.clear();
        }
        return true;
    }

}
