#include "./table.hpp"
#include "../include/string_utils.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

Table::Table(const std::string& name, const std::vector<ColumnDefinition>& columns)
    : name(name), columns(columns) {}

bool Table::insertRow(const std::vector<std::string>& values) {
    // Check if the number of values matches the number of columns
    if (values.size() != columns.size()) {
        return false;
    }
    
    // Create a new row
    Row row;
    row.values = values;
    
    // Add the row to the table
    rows.push_back(row);
    
    return true;
}

bool Table::insertRow(const std::vector<std::string>& columnNames, const std::vector<std::string>& values) {
    // Check if the number of column names matches the number of values
    if (columnNames.size() != values.size()) {
        return false;
    }
    
    // Create a new row with default values
    Row row;
    row.values.resize(columns.size());
    
    // Fill in the values for the specified columns
    for (size_t i = 0; i < columnNames.size(); i++) {
        int columnIndex = findColumnIndex(columnNames[i]);
        if (columnIndex == -1) {
            return false;  // Column not found
        }
        
        row.values[columnIndex] = values[i];
    }
    
    // Add the row to the table
    rows.push_back(row);
    
    return true;
}

std::vector<Row> Table::selectAll() const {
    return rows;
}

std::vector<Row> Table::selectWhere(const std::string& column, 
                                const std::string& op, 
                                const std::string& value) const {
    std::vector<Row> result;
    
    int columnIndex = findColumnIndex(column);
    if (columnIndex == -1) {
        return result;  // Column not found
    }
    
    for (const Row& row : rows) {
        if (compareValues(row.values[columnIndex], op, value)) {
            result.push_back(row);
        }
    }
    
    return result;
}

int Table::deleteWhere(const std::string& column, 
                    const std::string& op, 
                    const std::string& value) {
    int columnIndex = findColumnIndex(column);
    if (columnIndex == -1) {
        return 0;  // Column not found
    }
    
    size_t originalSize = rows.size();
    
    // Remove rows that match the condition
    rows.erase(
        std::remove_if(rows.begin(), rows.end(),
                    [this, columnIndex, op, value](const Row& row) {
                        return compareValues(row.values[columnIndex], op, value);
                    }),
        rows.end()
    );
    
    return originalSize - rows.size();
}

bool Table::saveToFile(std::ofstream& file) const {
    if (!file.is_open()) {
        return false;
    }
    
    // Write table name
    file << name << std::endl;
    
    // Write number of columns
    file << columns.size() << std::endl;
    
    // Write column definitions
    for (const auto& column : columns) {
        file << column.name << " ";
        
        switch (column.dataType) {
            case TokenType::INTEGER:
                file << "INTEGER";
                break;
            case TokenType::TEXT:
                file << "TEXT";
                break;
            case TokenType::REAL:
                file << "REAL";
                break;
            default:
                file << "UNKNOWN";
        }
        
        file << " " << (column.primaryKey ? "1" : "0")
             << " " << (column.notNull ? "1" : "0")
             << std::endl;
    }
    
    // Write number of rows
    file << rows.size() << std::endl;
    
    // Write row data
    for (const auto& row : rows) {
        for (size_t i = 0; i < row.values.size(); i++) {
            if (i > 0) {
                file << ",";
            }
            
            // Escape commas in the data
            std::string escapedValue = row.values[i];
            std::replace(escapedValue.begin(), escapedValue.end(), ',', '\\');
            
            file << escapedValue;
        }
        file << std::endl;
    }
    
    return true;
}

std::unique_ptr<Table> Table::loadFromFile(std::ifstream& file) {
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::string name;
    std::getline(file, name);
    
    size_t columnCount;
    file >> columnCount;
    file.ignore();  // Skip newline
    
    std::vector<ColumnDefinition> columns;
    
    for (size_t i = 0; i < columnCount; i++) {
        std::string line;
        std::getline(file, line);
        
        std::istringstream iss(line);
        std::string columnName, dataTypeStr;
        int primaryKeyFlag, notNullFlag;
        
        iss >> columnName >> dataTypeStr >> primaryKeyFlag >> notNullFlag;
        
        TokenType dataType;
        if (dataTypeStr == "INTEGER") {
            dataType = TokenType::INTEGER;
        } else if (dataTypeStr == "TEXT") {
            dataType = TokenType::TEXT;
        } else if (dataTypeStr == "REAL") {
            dataType = TokenType::REAL;
        } else {
            dataType = TokenType::INVALID;
        }
        
        columns.emplace_back(columnName, dataType, primaryKeyFlag != 0, notNullFlag != 0);
    }
    
    auto table = std::make_unique<Table>(name, columns);
    
    size_t rowCount;
    file >> rowCount;
    file.ignore();  // Skip newline
    
    for (size_t i = 0; i < rowCount; i++) {
        std::string line;
        std::getline(file, line);
        
        std::vector<std::string> values;
        std::string value;
        bool escaped = false;
        
        for (char c : line) {
            if (escaped) {
                value += c;
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == ',') {
                values.push_back(value);
                value.clear();
            } else {
                value += c;
            }
        }
        
        values.push_back(value);  // Last value
        
        table->insertRow(values);
    }
    
    return table;
}

int Table::findColumnIndex(const std::string& columnName) const {
    std::string trimmed = trim(columnName);
    for (size_t i = 0; i < columns.size(); i++) {
        if (trim(columns[i].name) == trimmed) {
            return static_cast<int>(i);
        }
    }
    
    return -1;  // Column not found
}

bool Table::compareValues(const std::string& value1, const std::string& op, const std::string& value2) const {
    // Check if both values are numeric
    bool isNumeric = true;
    double num1 = 0.0, num2 = 0.0;
    
    try {
        // Try to convert to numbers
        num1 = std::stod(value1);
        
        // Remove quotes if present
        std::string cleanValue2 = value2;
        if (cleanValue2.size() >= 2 && cleanValue2.front() == '\'' && cleanValue2.back() == '\'') {
            cleanValue2 = cleanValue2.substr(1, cleanValue2.size() - 2);
        }
        
        num2 = std::stod(cleanValue2);
    } catch (const std::exception&) {
        isNumeric = false;
    }
    
    if (isNumeric) {
        if (op == "=") {
            return num1 == num2;
        } else if (op == ">") {
            return num1 > num2;
        } else if (op == "<") {
            return num1 < num2;
        }
    } else {
        // String comparison
        // Remove quotes if present
        std::string cleanValue2 = value2;
        if (cleanValue2.size() >= 2 && cleanValue2.front() == '\'' && cleanValue2.back() == '\'') {
            cleanValue2 = cleanValue2.substr(1, cleanValue2.size() - 2);
        }
        
        if (op == "=") {
            return value1 == cleanValue2;
        } else if (op == ">") {
            return value1 > cleanValue2;
        } else if (op == "<") {
            return value1 < cleanValue2;
        }
    }
    
    return false;
}