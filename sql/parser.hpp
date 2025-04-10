#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <memory>
#include "token.hpp"
#include "tokenizer.hpp"

// Forward declarations for statement types
struct Statement;
struct CreateTableStatement;
struct InsertStatement;
struct SelectStatement;
struct DeleteStatement;

// Result of parsing
struct ParseResult {
    bool success;
    std::shared_ptr<Statement> statement;
    std::string errorMessage;
};

// Column definition
struct ColumnDefinition {
    std::string name;
    TokenType dataType;  // INTEGER, TEXT, REAL
    bool primaryKey;
    bool notNull;
    
    ColumnDefinition(const std::string& name, TokenType dataType, 
                     bool primaryKey = false, bool notNull = false)
        : name(name), dataType(dataType), primaryKey(primaryKey), notNull(notNull) {}
};

// Base statement class
struct Statement {
    enum class Type {
        CREATE_TABLE,
        INSERT,
        SELECT,
        DELETE,
        UPDATE,
        DROP_TABLE
    };
    
    Type type;
    
    explicit Statement(Type type) : type(type) {}
    virtual ~Statement() = default;
};

// CREATE TABLE statement
struct CreateTableStatement : public Statement {
    std::string tableName;
    std::vector<ColumnDefinition> columns;
    
    CreateTableStatement() : Statement(Type::CREATE_TABLE) {}
};

// INSERT statement
struct InsertStatement : public Statement {
    std::string tableName;
    std::vector<std::string> columnNames;
    std::vector<std::vector<std::string>> values;  // For multi-row insert
    
    InsertStatement() : Statement(Type::INSERT) {}
};

// SELECT statement
struct SelectStatement : public Statement {
    std::vector<std::string> columns;
    std::string tableName;
    std::string whereColumn;
    std::string whereOperator;
    std::string whereValue;
    bool hasWhere;
    
    SelectStatement() 
        : Statement(Type::SELECT), hasWhere(false) {}
};

// DELETE statement
struct DeleteStatement : public Statement {
    std::string tableName;
    std::string whereColumn;
    std::string whereOperator;
    std::string whereValue;
    bool hasWhere;
    
    DeleteStatement() 
        : Statement(Type::DELETE), hasWhere(false) {}
};

// Parser class
class Parser {
public:
    Parser() = default;
    
    // Parse a SQL query
    ParseResult parse(const std::string& query);
    
private:
    std::vector<Token> tokens;
    size_t current;  // Current token index
    
    // Helper methods
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    bool consume(TokenType type, const std::string& message);
    
    // Parsing methods
    ParseResult error(const std::string& message);
    std::shared_ptr<Statement> statement();
    std::shared_ptr<CreateTableStatement> createTable();
    std::shared_ptr<InsertStatement> insertStatement();
    std::shared_ptr<SelectStatement> selectStatement();
    std::shared_ptr<DeleteStatement> deleteStatement();
};

#endif // PARSER_HPP