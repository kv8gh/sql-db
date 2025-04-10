#ifndef TABLE_HPP
#define TABLE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include "../sql/parser.hpp"

// Structure to hold a single row in the table
struct Row {
    std::vector<std::string> values;
};

// Table class to manage table data
class Table {
public:
    Table(const std::string& name, const std::vector<ColumnDefinition>& columns);
    
    // Get table name
    std::string getName() const { return name; }
    
    // Get columns
    const std::vector<ColumnDefinition>& getColumns() const { return columns; }
    
    // Insert a new row
    bool insertRow(const std::vector<std::string>& values);
    bool insertRow(const std::vector<std::string>& columnNames, const std::vector<std::string>& values);
    
    // Select rows
    std::vector<Row> selectAll() const;
    std::vector<Row> selectWhere(const std::string& column, 
                               const std::string& op, 
                               const std::string& value) const;
    
    // Delete rows
    int deleteWhere(const std::string& column, 
                  const std::string& op, 
                  const std::string& value);
    
    // Save table to file
    bool saveToFile(std::ofstream& file) const;
    
    // Load table from file
    static std::unique_ptr<Table> loadFromFile(std::ifstream& file);
    
private:
    std::string name;
    std::vector<ColumnDefinition> columns;
    std::vector<Row> rows;
    
    // Helper method to find column index
    int findColumnIndex(const std::string& columnName) const;
    
    // Helper method to compare values
    bool compareValues(const std::string& value1, const std::string& op, const std::string& value2) const;
};

#endif // TABLE_HPP