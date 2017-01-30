#ifndef DATABASE_QUERYUPDATE_STORE_H
#define DATABASE_QUERYUPDATE_STORE_H

#include <vector>
#include <string>
#include <memory>

#include "Database_QueryBase.hpp"


namespace Query {
    struct QueryUpdate_Store : public QueryBase {
        std::string table;
        std::list<std::string> format;
        std::vector<std::string> data;
    };
    using Update = std::unique_ptr<QueryUpdate_Store>;
}

#endif
