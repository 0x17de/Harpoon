#ifndef DATABASE_QUERYDELETE_STORE_H
#define DATABASE_QUERYDELETE_STORE_H

#include <list>
#include <string>
#include <memory>

#include "Database_Query_Base.hpp"
#include "Database_QueryBase.hpp"


namespace Query {
    struct QueryDelete_Store : public QueryBase {
        std::string from;
        std::list<Join> on;
        std::unique_ptr<Query::Statement> filter;
        size_t limit;

        QueryDelete_Store();
    };
    using Delete = std::unique_ptr<QueryDelete_Store>;
}

#endif
