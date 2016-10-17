#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include <string>
#include <list>


namespace Database {

    enum class QueryType {
        LastId,
        Fetch,
        Insert,
        Delete,
        SetupTable
    };

    enum class OperationType {
        Order,
        CompareAnd,
        CompareOr,
        Assign,
        Join,
        Limit
    };

    class Operation {
        OperationType operation;
        std::string left, right;
        std::string extra;
        std::list<Operation> suboperations;
    public:
        Operation(OperationType operation,
                  const std::string& left = "",
                  const std::string& right = "",
                  const std::string& extra = "");

        Operation& add(OperationType operation,
                       const std::string& left = "",
                       const std::string& right = "",
                       const std::string& extra = "");

        OperationType getOperation() const;
        const std::string& getLeft() const;
        const std::string& getRight() const;
        const std::string& getExtra() const;
        const std::list<Operation>& getSuboperations() const;
    };

    class Query {
        QueryType type;
        std::string table;
        std::list<std::string> columns;
        std::list<Operation> operations;
    public:
        Query(QueryType type,
              const std::string& table,
              std::list<std::string>&& columns = {});

        Operation& add(OperationType operation,
                       const std::string& left =  "",
                       const std::string& right = "",
                       const std::string& extra = "");

        QueryType getType() const;
        const std::string& getTable() const;
        const std::list<std::string>& getColumns() const;
        const std::list<Operation>& getOperations() const;
    };

}


#endif
