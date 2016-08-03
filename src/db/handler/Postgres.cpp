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
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        void handleQuery(std::shared_ptr<IEvent> event);

        shared_ptr<soci::session> sqlSession;
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

    void Postgres_Impl::handleQuery(std::shared_ptr<IEvent> event) {
        auto db = event->as<EventDatabaseQuery>();
        for (const auto& query : db->getQueries()) {
            switch (query.getType()) {
            case Database::QueryType::SetupTable:
                {
                    auto once(sqlSession->once);
                    once << "CREATE TABLE IF NOT EXISTS " << query.getTable() << " "
                         << "(";
                    bool first = true;
                    for (auto& op : query.getOperations()) {
                        if (op.getOperation() != Database::OperationType::Assign)
                            continue;
                        if (!first)
                            once << ", ";
                        once << typeMap.at(op.getLeft()) << " " << op.getRight();
                    }
                    once << ")";
                }
                break;
            case Database::QueryType::Fetch:
#warning Postgres QueryType::Fetch stub
                break;
            case Database::QueryType::Insert:
#warning Postgres QueryType::Insert stub
                break;
            case Database::QueryType::Delete:
#warning Postgres QueryType::Delete stub
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
