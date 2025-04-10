#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include <memory>
#include <vector>
#include <string>
#include "../sql/parser.hpp"
#include "../storage/table.hpp"

// Result of executing a statement
struct ExecutionResult {
    bool success;
    std::string errorMessage;
    std::vector<std::vector<std::string>> rows;  // Result rows for SELECT
    std::vector<std::string> columnNames;        // Column names for SELECT
};

// Executor class to execute parsed statements
class Executor {
public:
    Executor() = default;
    
    // Execute a SQL statement
    ExecutionResult execute(
        const std::shared_ptr<Statement>& statement,
        std::vector<std::unique_ptr<Table>>& tables);
    
private:
    // Execute specific statement types
    ExecutionResult executeCreateTable(
        const std::shared_ptr<CreateTableStatement>& statement,
        std::vector<std::unique_ptr<Table>>& tables);
        
    ExecutionResult executeInsert(
        const std::shared_ptr<InsertStatement>& statement,
        std::vector<std::unique_ptr<Table>>& tables);
        
    ExecutionResult executeSelect(
        const std::shared_ptr<SelectStatement>& statement,
        std::vector<std::unique_ptr<Table>>& tables);
        
    ExecutionResult executeDelete(
        const std::shared_ptr<DeleteStatement>& statement,
        std::vector<std::unique_ptr<Table>>& tables);
        
    // Helper to find a table by name
    Table* findTable(const std::string& tableName, 
                   std::vector<std::unique_ptr<Table>>& tables);
};

#endif // EXECUTOR_HPP