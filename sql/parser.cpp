#include "./parser.hpp"
#include <algorithm>
#include <iostream>


ParseResult Parser::parse(const std::string& query) {
    std::cout << "Parsing: " << query << std::endl;
    Tokenizer tokenizer(query);
    tokens = tokenizer.scanTokens();
    std::cout << "Tokens: " << std::endl;
    for (const auto& t : tokens) {
        std::cout << t.toString() << std::endl;
    }
    current = 0;
    
    try {
        std::shared_ptr<Statement> stmt = statement();
        
        // Check for trailing tokens
        if (!isAtEnd()) {
            return {false, nullptr, "Unexpected tokens after statement"};
        }
        
        return {true, stmt, ""};
    } catch (const std::string& error) {
        return {false, nullptr, error};
    }
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    std:: cout<<"outside if statement";
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return true;
    }
    throw message;
}

ParseResult Parser::error(const std::string& message) {
    return {false, nullptr, message};
}

std::shared_ptr<Statement> Parser::statement() {
    if (match({TokenType::CREATE})) {
        return createTable();
    } else if (match({TokenType::INSERT})) {
        return insertStatement();
    } else if (match({TokenType::SELECT})) {
        return selectStatement();
    } else if (match({TokenType::DELETE})) {
        return deleteStatement();
    }
    
    throw "Unexpected token: " + peek().lexeme;
}

std::shared_ptr<CreateTableStatement> Parser::createTable() {
    consume(TokenType::TABLE, "Expected 'TABLE' after 'CREATE'");
    
    auto stmt = std::make_shared<CreateTableStatement>();
    
    // Parse table name
    consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = previous().lexeme;
    
    // Parse column definitions
    consume(TokenType::LEFT_PAREN, "Expected '(' after table name");
    
    // Parse first column
    consume(TokenType::IDENTIFIER, "Expected column name");
    std::string colName = previous().lexeme;
    
    // Parse column type
    if (!match({TokenType::INTEGER, TokenType::TEXT, TokenType::REAL})) {
        throw "Expected column type (INTEGER, TEXT, REAL)";
    }
    TokenType colType = previous().type;
    
    bool isPrimary = false;
    bool isNotNull = false;
    
    // Check for column constraints
    while (match({TokenType::IDENTIFIER})) {
        std::string constraint = previous().lexeme;
        std::transform(constraint.begin(), constraint.end(), constraint.begin(), ::toupper);
        
        if (constraint == "PRIMARY" && match({TokenType::IDENTIFIER})) {
            std::string keyWord = previous().lexeme;
            std::transform(keyWord.begin(), keyWord.end(), keyWord.begin(), ::toupper);
            if (keyWord == "KEY") {
                isPrimary = true;
            } else {
                throw "Expected 'KEY' after 'PRIMARY'";
            }
        } else if (constraint == "NOT" && match({TokenType::IDENTIFIER})) {
            std::string nullWord = previous().lexeme;
            std::transform(nullWord.begin(), nullWord.end(), nullWord.begin(), ::toupper);
            if (nullWord == "NULL") {
                isNotNull = true;
            } else {
                throw "Expected 'NULL' after 'NOT'";
            }
        }
    }
    
    // Add first column
    stmt->columns.emplace_back(colName, colType, isPrimary, isNotNull);
    
    // Parse additional columns
    while (match({TokenType::COMMA})) {
        consume(TokenType::IDENTIFIER, "Expected column name");
        colName = previous().lexeme;
        
        if (!match({TokenType::INTEGER, TokenType::TEXT, TokenType::REAL})) {
            throw "Expected column type (INTEGER, TEXT, REAL)";
        }
        colType = previous().type;
        
        isPrimary = false;
        isNotNull = false;
        
        // Check for column constraints
        while (match({TokenType::IDENTIFIER})) {
            std::string constraint = previous().lexeme;
            std::transform(constraint.begin(), constraint.end(), constraint.begin(), ::toupper);
            
            if (constraint == "PRIMARY" && match({TokenType::IDENTIFIER})) {
                std::string keyWord = previous().lexeme;
                std::transform(keyWord.begin(), keyWord.end(), keyWord.begin(), ::toupper);
                if (keyWord == "KEY") {
                    isPrimary = true;
                } else {
                    throw "Expected 'KEY' after 'PRIMARY'";
                }
            } else if (constraint == "NOT" && match({TokenType::IDENTIFIER})) {
                std::string nullWord = previous().lexeme;
                std::transform(nullWord.begin(), nullWord.end(), nullWord.begin(), ::toupper);
                if (nullWord == "NULL") {
                    isNotNull = true;
                } else {
                    throw "Expected 'NULL' after 'NOT'";
                }
            }
        }
        
        // Add column
        stmt->columns.emplace_back(colName, colType, isPrimary, isNotNull);
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after column definitions");
    consume(TokenType::SEMICOLON, "Expected ';' after CREATE TABLE statement");
    
    return stmt;
}

std::shared_ptr<InsertStatement> Parser::insertStatement() {
    consume(TokenType::INTO, "Expected 'INTO' after 'INSERT'");
    
    auto stmt = std::make_shared<InsertStatement>();
    
    // Parse table name
    consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = previous().lexeme;
    
    // Check if column names are specified
    if (match({TokenType::LEFT_PAREN})) {
        // Parse column names
        consume(TokenType::IDENTIFIER, "Expected column name");
        stmt->columnNames.push_back(previous().lexeme);
        
        while (match({TokenType::COMMA})) {
            consume(TokenType::IDENTIFIER, "Expected column name");
            stmt->columnNames.push_back(previous().lexeme);
        }
        
        consume(TokenType::RIGHT_PAREN, "Expected ')' after column names");
    }
    
    consume(TokenType::VALUES, "Expected 'VALUES' keyword");
    
    // Parse values
    consume(TokenType::LEFT_PAREN, "Expected '(' after VALUES");
    
    std::vector<std::string> rowValues;
    
    // Parse first value
    if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
        rowValues.push_back(previous().lexeme);
    } else if (match({TokenType::STRING_LITERAL})) {
        rowValues.push_back("'" + previous().lexeme + "'");
    } else {
        throw "Expected value";
    }
    
    // Parse additional values
    while (match({TokenType::COMMA})) {
        if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
            rowValues.push_back(previous().lexeme);
        } else if (match({TokenType::STRING_LITERAL})) {
            rowValues.push_back("'" + previous().lexeme + "'");
        } else {
            throw "Expected value";
        }
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after values");
    
    // Add the row values
    stmt->values.push_back(rowValues);
    
    // Check for additional value rows
    while (match({TokenType::COMMA})) {
        consume(TokenType::LEFT_PAREN, "Expected '(' for values");
        
        rowValues.clear();
        
        // Parse first value
        if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
            rowValues.push_back(previous().lexeme);
        } else if (match({TokenType::STRING_LITERAL})) {
            rowValues.push_back("'" + previous().lexeme + "'");
        } else {
            throw "Expected value";
        }
        
        // Parse additional values
        while (match({TokenType::COMMA})) {
            if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
                rowValues.push_back(previous().lexeme);
            } else if (match({TokenType::STRING_LITERAL})) {
                rowValues.push_back("'" + previous().lexeme + "'");
            } else {
                throw "Expected value";
            }
        }
        
        consume(TokenType::RIGHT_PAREN, "Expected ')' after values");
        
        // Add the row values
        stmt->values.push_back(rowValues);
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after INSERT statement");
    
    return stmt;
}

std::shared_ptr<SelectStatement> Parser::selectStatement() {
    auto stmt = std::make_shared<SelectStatement>();
    
    // Parse columns
    if (match({TokenType::STAR})) {
        stmt->columns.push_back("*");
    } else {
        consume(TokenType::IDENTIFIER, "Expected column name or '*'");
        stmt->columns.push_back(previous().lexeme);
        
        while (match({TokenType::COMMA})) {
            consume(TokenType::IDENTIFIER, "Expected column name");
            stmt->columns.push_back(previous().lexeme);
        }
    }
    
    consume(TokenType::FROM, "Expected 'FROM' after SELECT columns");
    
    // Parse table name
    consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = previous().lexeme;
    
    // Parse WHERE clause if present
    if (match({TokenType::WHERE})) {
        stmt->hasWhere = true;
        
        // Parse column name
        consume(TokenType::IDENTIFIER, "Expected column name in WHERE clause");
        stmt->whereColumn = previous().lexeme;
        
        // Parse operator
        if (match({TokenType::EQUALS})) {
            stmt->whereOperator = "=";
        } else if (match({TokenType::GREATER})) {
            stmt->whereOperator = ">";
        } else if (match({TokenType::LESS})) {
            stmt->whereOperator = "<";
        } else {
            throw "Expected operator in WHERE clause";
        }
        
        // Parse value
        if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
            stmt->whereValue = previous().lexeme;
        } else if (match({TokenType::STRING_LITERAL})) {
            stmt->whereValue = "'" + previous().lexeme + "'";
        } else {
            throw "Expected value in WHERE clause";
        }
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after SELECT statement");
    
    return stmt;
}

std::shared_ptr<DeleteStatement> Parser::deleteStatement() {
    consume(TokenType::FROM, "Expected 'FROM' after DELETE");
    
    auto stmt = std::make_shared<DeleteStatement>();
    
    // Parse table name
    consume(TokenType::IDENTIFIER, "Expected table name");
    stmt->tableName = previous().lexeme;
    
    // Parse WHERE clause if present
    if (match({TokenType::WHERE})) {
        stmt->hasWhere = true;
        
        // Parse column name
        consume(TokenType::IDENTIFIER, "Expected column name in WHERE clause");
        stmt->whereColumn = previous().lexeme;
        
        // Parse operator
        if (match({TokenType::EQUALS})) {
            stmt->whereOperator = "=";
        } else if (match({TokenType::GREATER}))
        // Parse operator
        if (match({TokenType::EQUALS})) {
            stmt->whereOperator = "=";
        } else if (match({TokenType::GREATER})) {
            stmt->whereOperator = ">";
        } else if (match({TokenType::LESS})) {
            stmt->whereOperator = "<";
        } else {
            throw "Expected operator in WHERE clause";
        }
        
        // Parse value
        if (match({TokenType::INTEGER_LITERAL, TokenType::FLOAT_LITERAL})) {
            stmt->whereValue = previous().lexeme;
        } else if (match({TokenType::STRING_LITERAL})) {
            stmt->whereValue = "'" + previous().lexeme + "'";
        } else {
            throw "Expected value in WHERE clause";
        }
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after DELETE statement");
    
    return stmt;
}