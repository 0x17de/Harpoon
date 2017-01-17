#ifndef DATABASE_QUERYINSERT_STORE_H
#define DATABASE_QUERYINSERT_STORE_H

#include <vector>
#include <string>
#include <memory>


namespace Query {
    struct QueryInsert_Store {
        std::string into;
        std::list<JoinStatement> on;
        std::list<std::string> format;
        std::vector<std::string> data;
    };
    using Insert = std::unique_ptr<QueryInsert_Store>;
}

#endif
