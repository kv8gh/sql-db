#include "executor/executor.hpp"
#include <iostream>

ExecutionResult Executor::execute(
    const std::shared_ptr<Statement>& statement,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    switch (statement->type) {
        case Statement::Type::CREATE_TABLE:
            return executeCreateTable(
                std::static_pointer_cast<CreateTableStatement>(statement),
                tables);
            
        case Statement::Type::INSERT:
            return executeInsert(
                std::static_pointer_cast<InsertStatement>(statement),
                tables);
            
        case Statement::Type::SELECT:
            return executeSelect(
                std::static_pointer_cast<SelectStatement>(statement),
                tables);
            
        case Statement::Type::DELETE:
            return executeDelete(
                std::static_pointer_cast<DeleteStatement>(statement),
                tables);
            
        default:
            return {false, "Unsupported statement type", {}, {}};
    }
}

ExecutionResult Executor::executeCreateTable(
    const std::shared_ptr<CreateTableStatement>& statement,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    // Check if table already exists
    if (findTable(statement->tableName, tables) != nullptr) {
        return {false, "Table already exists: " + statement->tableName, {}, {}};
    }
    
    // Create the new table
    tables.push_back(std::make_unique<Table>(statement->tableName, statement->columns));
    
    std::cout << "Table created: " << statement->tableName << std::endl;
    return {true, "", {}, {}};
}

ExecutionResult Executor::executeInsert(
    const std::shared_ptr<InsertStatement>& statement,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    // Find the table
    Table* table = findTable(statement->tableName, tables);
    if (table == nullptr) {
        return {false, "Table not found: " + statement->tableName, {}, {}};
    }
    
    // Insert each row
    for (const auto& values : statement->values) {
        bool success;
        
        if (statement->columnNames.empty()) {
            // No column names specified, use direct insertion
            success = table->insertRow(values);
        } else {
            // Column names specified
            success = table->insertRow(statement->columnNames, values);
        }
        
        if (!success) {
            return {false, "Failed to insert row into table: " + statement->tableName, {}, {}};
        }
    }
    
    std::cout << statement->values.size() << " row(s) inserted into " << statement->tableName << std::endl;
    return {true, "", {}, {}};
}

ExecutionResult Executor::executeSelect(
    const std::shared_ptr<SelectStatement>& statement,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    // Find the table
    Table* table = findTable(statement->tableName, tables);
    if (table == nullptr) {
        return {false, "Table not found: " + statement->tableName, {}, {}};
    }
    
    std::vector<Row> rows;
    
    // Apply WHERE clause if present
    if (statement->hasWhere) {
        rows = table->selectWhere(
            statement->whereColumn,
            statement->whereOperator,
            statement->whereValue
        );
    } else {
        rows = table->selectAll();
    }
    
    // Prepare result
    ExecutionResult result = {true, "", {}, {}};
    
    // Get column information
    const auto& columns = table->getColumns();
    
    // Determine which columns to include
    std::vector<int> columnIndices;
    if (statement->columns.size() == 1 && statement->columns[0] == "*") {
        // Select all columns
        for (size_t i = 0; i < columns.size(); i++) {
            columnIndices.push_back(static_cast<int>(i));
            result.columnNames.push_back(columns[i].name);
        }
    } else {
        // Select specific columns
        for (const auto& colName : statement->columns) {
            int colIndex = -1;
            for (size_t i = 0; i < columns.size(); i++) {
                if (columns[i].name == colName) {
                    colIndex = static_cast<int>(i);
                    break;
                }
            }
            
            if (colIndex == -1) {
                return {false, "Column not found: " + colName, {}, {}};
            }
            
            columnIndices.push_back(colIndex);
            result.columnNames.push_back(colName);
        }
    }
    
    // Extract values for the selected columns
    for (const auto& row : rows) {
        std::vector<std::string> resultRow;
        for (int index : columnIndices) {
            resultRow.push_back(row.values[index]);
        }
        result.rows.push_back(resultRow);
    }
    
    std::cout << rows.size() << " row(s) returned" << std::endl;
    
    // Print the result to console
    // Print header
    for (size_t i = 0; i < result.columnNames.size(); i++) {
        if (i > 0) {
            std::cout << " | ";
        }
        std::cout << result.columnNames[i];
    }
    std::cout << std::endl;
    
    // Print separator
    for (size_t i = 0; i < result.columnNames.size(); i++) {
        if (i > 0) {
            std::cout << "-+-";
        }
        std::cout << std::string(result.columnNames[i].length(), '-');
    }
    std::cout << std::endl;
    
    // Print rows
    for (const auto& row : result.rows) {
        for (size_t i = 0; i < row.size(); i++) {
            if (i > 0) {
                std::cout << " | ";
            }
            std::cout << row[i];
        }
        std::cout << std::endl;
    }
    
    return result;
}

ExecutionResult Executor::executeDelete(
    const std::shared_ptr<DeleteStatement>& statement,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    // Find the table
    Table* table = findTable(statement->tableName, tables);
    if (table == nullptr) {
        return {false, "Table not found: " + statement->tableName, {}, {}};
    }
    
    int rowsDeleted = 0;
    
    // Apply WHERE clause if present
    if (statement->hasWhere) {
        rowsDeleted = table->deleteWhere(
            statement->whereColumn,
            statement->whereOperator,
            statement->whereValue
        );
    } else {
        // Delete all rows (dangerous!)
        rowsDeleted = table->deleteWhere("", "=", "");  // This should delete all rows
    }
    
    std::cout << rowsDeleted << " row(s) deleted from " << statement->tableName << std::endl;
    return {true, "", {}, {}};
}

Table* Executor::findTable(
    const std::string& tableName,
    std::vector<std::unique_ptr<Table>>& tables) {
    
    for (auto& table : tables) {
        if (table->getName() == tableName) {
            return table.get();
        }
    }
    
    return nullptr;
}