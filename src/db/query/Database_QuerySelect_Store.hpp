#ifndef DATABASE_QUERYSELECT_STORE_H
#define DATABASE_QUERYSELECT_STORE_H

#include <list>
#include <string>
#include <memory>

#include "Database_QueryBase.hpp"


namespace Query {
    struct QuerySelect_Store : public QueryBase {
        std::list<std::string> what;
        std::string from;
        std::list<Join> on;
        std::unique_ptr<Query::Statement> filter;
        std::list<OrderStatement> order;
    };
    using Select = std::unique_ptr<QuerySelect_Store>;
}

#endif
