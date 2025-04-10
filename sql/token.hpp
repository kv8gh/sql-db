#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    // Keywords
    SELECT,
    INSERT,
    UPDATE,
    DELETE,
    CREATE,
    DROP,
    TABLE,
    FROM,
    WHERE,
    INTO,
    VALUES,
    SET,
    
    // Data types
    INTEGER,
    TEXT,
    REAL,
    
    // Operators
    EQUALS,
    STAR,
    COMMA,
    SEMICOLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    PLUS,
    MINUS,
    GREATER,
    LESS,
    
    // Literals
    IDENTIFIER,
    STRING_LITERAL,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    
    // Special
    EOF_TOKEN,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    
    Token(TokenType type, const std::string& lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
        
    std::string toString() const;
};

#endif // TOKEN_HPP