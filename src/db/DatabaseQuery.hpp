#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include <string>
#include <list>


namespace Database {

    enum class OperationType {
        CompareAnd,
        CompareOr,
        Assign,
        Join
    };

    class Operation {
        OperationType operation;
        std::string left, right;
        std::string extra;
    public:
        Operation(OperationType operation,
                  const std::string& left,
                  const std::string& right,
                  const std::string& extra = "");

        OperationType getOperation() const;
        const std::string& getLeft() const;
        const std::string& getRight() const;
        const std::string& getExtra() const;
    };

    class Query {
        std::string table;
        std::list<std::string> columns;
        std::list<Operation> operations;
    public:
        Query(const std::string& table,
              std::list<std::string>&& columns);

        void add(OperationType operation,
                 const std::string& left,
                 const std::string& right,
                 const std::string& extra = "");

        const std::string& getTable() const;
        const std::list<std::string>& getColumns() const;
        const std::list<Operation>& getOperations() const;
    };

}

#endif
