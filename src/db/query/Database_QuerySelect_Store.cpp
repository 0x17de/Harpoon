#include <limits>
#include "Database_QuerySelect_Store.hpp"


namespace Query {
    QuerySelect_Store::QuerySelect_Store()
        : limit(std::numeric_limits<size_t>::max())
    { }
}
