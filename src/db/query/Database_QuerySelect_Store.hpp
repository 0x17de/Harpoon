#ifndef DATABASE_QUERYSELECT_STORE_H
#define DATABASE_QUERYSELECT_STORE_H

#include <list>
#include <string>
#include <memory>


namespace Query {
    struct QuerySelect_Store {
        std::list<std::string> what;
        std::string from;
        std::list<JoinStatement> on;
        std::unique_ptr<Query::Statement> filter;
        std::list<OrderStatement> order;
    };
    using Select = std::unique_ptr<QuerySelect_Store>;
}

#endif
