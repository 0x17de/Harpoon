#include "DatabaseQuery.hpp"


namespace Database {

    Operation::Operation(OperationType operation,
                         const std::string& left,
                         const std::string& right,
                         const std::string& extra)
        : operation{operation}
        , left{left}
        , right{right}
        , extra{extra}
    {
    }

    const std::string& Operation::getLeft() const {
        return left;
    }

    const std::string& Operation::getRight() const {
        return right;
    }

    const std::string& Operation::getExtra() const {
        return extra;
    }

    Query::Query(const std::string& table,
                 std::list<std::string>&& columns)
        : table{table}
        , columns{columns}
    {
    }

    void Query::add(OperationType operation,
                    const std::string& left,
                    const std::string& right,
                    const std::string& extra) {
        operations.emplace_back(operation, left, right);
    }

    const std::string& Query::getTable() const {
        return table;
    }

    const std::list<std::string>& Query::getColumns() const {
        return columns;
    }

    const std::list<Operation>& Query::getOperations() const {
        return operations;
    }


}
