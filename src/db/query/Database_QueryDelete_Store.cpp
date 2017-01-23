#include <limits>
#include "Database_QueryDelete_Store.hpp"


namespace Query {
    QueryDelete_Store::QueryDelete_Store()
        : limit(std::numeric_limits<size_t>::max())
    { }
}
