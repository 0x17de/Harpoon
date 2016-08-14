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

    Operation& Operation::add(OperationType operation,
                    const std::string& left,
                    const std::string& right,
                    const std::string& extra) {
        suboperations.emplace_back(operation, left, right);
        return suboperations.back();
    }

    OperationType Operation::getOperation() const {
        return operation;
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

    const std::list<Operation>& Operation::getSuboperations() const {
        return suboperations;
    }

    Query::Query(QueryType type,
                 const std::string& table,
                 std::list<std::string>&& columns)
        : type{type}
        , table{table}
        , columns{columns}
    {
    }

    Operation& Query::add(OperationType operation,
                          const std::string& left,
                          const std::string& right,
                          const std::string& extra) {
        operations.emplace_back(operation, left, right, extra);
        return operations.back();
    }

    QueryType Query::getType() const {
        return type;
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
