#ifndef DB_ENGINE_HPP
#define DB_ENGINE_HPP

#include <string>
#include <vector>
#include <memory>
#include "../sql/parser.hpp"
#include "../executor/executor.hpp"
#include "../storage/table.hpp"

/**
 * Main database engine class that coordinates the parser, executor, and storage
 */
class DBEngine {
public:
    DBEngine();
    ~DBEngine();
    
    // Open a database file
    bool openDatabase(const std::string& filename);
    
    // Execute a SQL query
    ExecutionResult executeQuery(const std::string& query);
    
    // List all tables in the database
    void listTables();
    
private:
    std::string databaseFilename;
    bool isDatabaseOpen;
    std::unique_ptr<Parser> parser;
    std::unique_ptr<Executor> executor;
    std::vector<std::unique_ptr<Table>> tables;
};

#endif // DB_ENGINE_HPP