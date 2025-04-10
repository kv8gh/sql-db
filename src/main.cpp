#include <iostream>
#include <string>
#include "../include/db_engine.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Welcome to MiniDB - A Simple SQLite Clone\n";
    std::cout << "Enter .help for usage hints.\n";
    
    // Create database instance
    DBEngine db;
    
    // If a database file is provided as argument, open it
    if (argc > 1) {
        std::string filename = argv[1];
        if (!db.openDatabase(filename)) {
            std::cerr << "Failed to open database file: " << filename << std::endl;
            return 1;
        }
        std::cout << "Opened database: " << filename << std::endl;
    }
    
    // Read-Evaluate-Print Loop (REPL)
    std::string input;
    while (true) {
        std::cout << "db > ";
        std::getline(std::cin, input);
        
        // Handle meta-commands (commands that start with a dot)
        if (!input.empty() && input[0] == '.') {
            if (input == ".exit") {
                break;
            } else if (input == ".help") {
                std::cout << "Special commands:\n"
                          << "  .exit      Exit the program\n"
                          << "  .help      Show this message\n"
                          << "  .open FILE Open a database file\n"
                          << "  .tables    Show all tables\n";
                continue;
            } else if (input.substr(0, 5) == ".open" && input.length() > 6) {
                std::string filename = input.substr(6);
                if (!db.openDatabase(filename)) {
                    std::cerr << "Failed to open database file: " << filename << std::endl;
                }
                std::cout << "Opened database: " << filename << std::endl;
                continue;
            } else if (input == ".tables") {
                db.listTables();
                continue;
            } else {
                std::cout << "Unknown command: " << input << std::endl;
                continue;
            }
        }
        
        // Process SQL query
        if (!input.empty()) {
            ExecutionResult result = db.executeQuery(input);
            if (!result.success) {
                std::cout << "Error: " << result.errorMessage << std::endl;
            }
        }
    }
    
    std::cout << "Goodbye!\n";
    return 0;
}