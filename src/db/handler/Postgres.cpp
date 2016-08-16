#include "Postgres.hpp"
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
        EventQueue* appQueue;
        shared_ptr<soci::session> sqlSession;
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        void handleQuery(std::shared_ptr<IEvent> event);

        void query_setupTable(const Database::Query& query, EventDatabaseResult* result);
        void query_insert(const Database::Query& query, EventDatabaseResult* result);
        void query_fetch(const Database::Query& query, EventDatabaseResult* result);
        void query_delete(const Database::Query& query, EventDatabaseResult* result);

        void query_scanOperations(const Database::Query& query,
                                  std::list<Database::Operation const *>& join,
                                  Database::Operation const *& where,
                                  Database::Operation const *& limit);
        template<class T>
        void query_handleWhere(T& temp_type,
                               const Database::Operation& op,
                               size_t& index,
                               std::vector<size_t>* ids = 0);

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

    Postgres::~Postgres() {
    }

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    const map<string, string> Postgres_Impl::typeMap {
        {"id", "serial"},
        {"time", "timestamp"},
        {"int", "integer"},
        {"text", "text"},
        {"bool", "boolean"}
    };

    void Postgres_Impl::query_setupTable(const Database::Query& query, EventDatabaseResult* result) {
        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        for (auto& op : join) {
            auto q = sqlSession->once << "CREATE TABLE IF NOT EXISTS "
                                      << op->getExtra() << " ("
                                      << op->getLeft() << "_id serial, "
                                      << op->getLeft() << " " << typeMap.at(op->getRight())
                                      << ")";
        }

        {
            auto q = sqlSession->once << "CREATE TABLE IF NOT EXISTS " << query.getTable() << " (";
            bool first = true;
            for (auto& op : query.getOperations()) {
                if (op.getOperation() != Database::OperationType::Assign)
                    continue;
                if (first) {
                    first = false;
                } else {
                    q << ", ";
                }
                q << op.getLeft() << " " << typeMap.at(op.getRight());
            }
            for (auto& op : join) {
                if (first) {
                    first = false;
                } else {
                    q << ", ";
                }
                q << op->getLeft() << "_ref integer";
            }
            q << ")";
        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_insert(const Database::Query& query, EventDatabaseResult* result) {
        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        std::vector<size_t> ids;
        size_t joinIndex = 0;
        for (auto& op : join) {
            size_t id = 0;
            sqlSession->once << "SELECT " << op->getLeft() << "_id FROM "
                             << op->getExtra() << " WHERE "
                             << op->getLeft() << " = :sel" << joinIndex, soci::into(id), soci::use(op->getRight());
            if (!sqlSession->got_data()) { // not found
                long lid;
                stringstream ss;
                ss << op->getExtra() << "_" << op->getLeft() <<  "_id_seq";
                if (sqlSession->get_next_sequence_value(ss.str(), lid)) {
                    sqlSession->once << "INSERT INTO "
                                     << op->getExtra() << "(" << op->getLeft() << "_id, " << op->getLeft() << ")"
                                     << " VALUES (:insId" << joinIndex << ", :ins" << joinIndex << ")", soci::use(lid), soci::use(op->getRight());
                    id = lid;
                } else {
                    sqlSession->once << "INSERT INTO "
                                     << op->getExtra() << "(" << op->getLeft() << ")"
                                     << " VALUES (:ins" << joinIndex << ")", soci::use(op->getRight());
                    if (sqlSession->get_last_insert_id(op->getExtra(), lid))
                        id = lid;
                }
            }
            ids.push_back(id);
            ++joinIndex;
        }

        {
            auto q = sqlSession->once << "INSERT INTO " << query.getTable() << " " << "(";
            bool first = true;
            for (auto& col : query.getColumns()) {
                if (first) {
                    first = false;
                } else {
                    q << ", ";
                }
                q << col;
            }
            q << ") VALUES (";

            auto& operations = query.getOperations();
            size_t columnCount = query.getColumns().size();
            size_t index = 0;
            for (auto& op : operations) {
                if (op.getOperation() != Database::OperationType::Assign)
                    continue;
                if (index > 0 && index % columnCount == 0)
                    q << ")";
                if (index != 0)
                    q << ", ";
                if (index > 0 && index % columnCount == 0)
                    q << "(";
                q << ":op" << index;
                if (op.getExtra().size() > 0) {
                    size_t idsIndex;
                    istringstream(op.getExtra()) >> idsIndex;
                    size_t& id = ids.at(idsIndex); // needs to live till the end of the request
                    q, soci::use(id);
                } else {
                    q, soci::use(op.getLeft());
                }
                ++index;
            }
            q << ")";
        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_fetch(const Database::Query& query, EventDatabaseResult* result) {
#pragma message "Postgres QueryType::Fetch stub"
        auto& columns = query.getColumns();
        vector<string> temp(columns.size());

        auto q = sqlSession->prepare << "SELECT ";
        bool first = true;
        for (auto& column : query.getColumns()) {
            if (first) {
                first = false;
            } else {
                q << ", ";
            }
            q << column;
        }
        q << " FROM " << query.getTable();

        Database::Operation const *where = 0, *limit = 0;
        std::list<Database::Operation const *> join;
        query_scanOperations(query, join, where, limit);

        if (where) {
            q << " WHERE ";
            size_t index;
            query_handleWhere(q, *where, index);
        }

        for (size_t i = 0; i < columns.size(); ++i)
            q, soci::into(temp[i]);

        soci::statement st = q;
        st.execute();
        while (st.fetch()) {
            for (auto& s : temp)
                result->addResult(s);
        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_delete(const Database::Query& query, EventDatabaseResult* result) {
#pragma message "Postgres QueryType::Delete stub"
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
                return; // ERR: something went wrong
            }
            ++joinIndex;
        }

        auto q = sqlSession->once << "DELETE FROM " << query.getTable();

        if (where) {
            q << " WHERE ";
            size_t index;
            query_handleWhere(q, *where, index, &ids);
        }

        result->setSuccess(true);
    }

    void Postgres_Impl::query_scanOperations(const Database::Query& query,
                                             std::list<Database::Operation const *>& join,
                                             Database::Operation const *& where,
                                             Database::Operation const *& limit) {
        for (auto& op : query.getOperations()) {
            switch (op.getOperation()) {
            case Database::OperationType::Assign:
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
            default:
                break;
            }
        }
    }

    template<class T>
    void Postgres_Impl::query_handleWhere(T& q,
                                          const Database::Operation& op,
                                          size_t& index,
                                          std::vector<size_t>* ids) {
        static_assert(is_same<T, soci::details::once_temp_type>::value
                      || is_same<T, soci::details::prepare_temp_type>::value,
                      "Type must be once_temp_type or prepare_temp_type");
        if (op.getOperation() == Database::OperationType::Assign) {
            q << op.getLeft() << " = " << ":where" << index;
            if (op.getExtra().size() > 0) {
                size_t idsIndex;
                istringstream(op.getExtra()) >> idsIndex;
                size_t& id = ids->at(idsIndex); // needs to live till the end of the request
                q, soci::use(id);
            } else {
                q, soci::use(op.getRight());
            }
            return;
        }

        auto& subOps = op.getSuboperations();
        if (subOps.size() == 0) return;

        bool first = true;
        q << "(";
        for (auto& subOp : subOps) {
            if (first) {
                first = false;
            } else {
                if (op.getOperation() == Database::OperationType::CompareOr) {
                    q << " OR ";
                } else {
                    q << " AND ";
                }
            }
            query_handleWhere(q, subOp, index, ids);
        }
        q << ")";
    }

    void Postgres_Impl::handleQuery(std::shared_ptr<IEvent> event) {
        EventDatabaseQuery* query = event->as<EventDatabaseQuery>();
        auto result = make_shared<EventDatabaseResult>(query->getEventOrigin());
        auto db = event->as<EventDatabaseQuery>();

        for (const auto& query : db->getQueries()) {
            switch (query.getType()) {
            case Database::QueryType::SetupTable:
                query_setupTable(query, result.get());
                break;
            case Database::QueryType::Fetch:
                query_fetch(query, result.get());
                break;
            case Database::QueryType::Insert:
                query_insert(query, result.get());
                break;
            case Database::QueryType::Delete:
                query_delete(query, result.get());
                break;
            default:
                break;
            }
        }

        query->getTarget()->sendEvent(result);
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
