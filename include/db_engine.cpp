#include "../include/db_engine.hpp"
#include <iostream>
#include <fstream>

DBEngine::DBEngine() : isDatabaseOpen(false) {
    parser = std::make_unique<Parser>();
    executor = std::make_unique<Executor>();
}

DBEngine::~DBEngine() {
    // Save tables and close database if open
    if (isDatabaseOpen) {
        // Implementation for saving database state would go here
    }
}

bool DBEngine::openDatabase(const std::string& filename) {
    // Close previous database if open
    if (isDatabaseOpen) {
        // Save current state
        tables.clear();
    }
    
    // Try to open the database file
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        // If the file doesn't exist, create a new one
        std::ofstream newFile(filename, std::ios::binary);
        if (!newFile.is_open()) {
            return false;
        }
        newFile.close();
        
        // New empty database
        databaseFilename = filename;
        isDatabaseOpen = true;
        return true;
    }
    
    // Read database file and load table metadata
    // This is a simplified version - a real implementation would 
    // read the database file format and load all tables
    
    databaseFilename = filename;
    isDatabaseOpen = true;
    
    // For now, we'll just create an empty database
    return true;
}

ExecutionResult DBEngine::executeQuery(const std::string& query) {
    if (!isDatabaseOpen) {
        return ExecutionResult{false, "No database is open"};
    }
    
    // Parse the SQL query
    ParseResult parseResult = parser->parse(query);
    if (!parseResult.success) {
        return ExecutionResult{false, parseResult.errorMessage};
    }
    
    // Execute the parsed statement
    ExecutionResult result = executor->execute(parseResult.statement, tables);
    
    return result;
}

void DBEngine::listTables() {
    if (!isDatabaseOpen) {
        std::cout << "No database is open" << std::endl;
        return;
    }
    
    if (tables.empty()) {
        std::cout << "No tables found" << std::endl;
        return;
    }
    
    std::cout << "Tables:" << std::endl;
    for (const auto& table : tables) {
        std::cout << "  " << table->getName() << std::endl;
    }
}