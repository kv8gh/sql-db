#include "../include/sql/token.hpp"
#include <cctype>
#include <unordered_map>

// Keyword map
static const std::unordered_map<std::string, TokenType> keywords = {
    {"select", TokenType::SELECT},
    {"insert", TokenType::INSERT},
    {"update", TokenType::UPDATE},
    {"delete", TokenType::DELETE},
    {"create", TokenType::CREATE},
    {"drop", TokenType::DROP},
    {"table", TokenType::TABLE},
    {"from", TokenType::FROM},
    {"where", TokenType::WHERE},
    {"into", TokenType::INTO},
    {"values", TokenType::VALUES},
    {"set", TokenType::SET},
    {"integer", TokenType::INTEGER},
    {"text", TokenType::TEXT},
    {"real", TokenType::REAL}
};

Tokenizer::Tokenizer(const std::string& source)
    : source(source), start(0), current(0), line(1) {}

std::vector<Token> Tokenizer::scanTokens() {
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        // Beginning of the next lexeme
        start = current;
        Token token = scanToken();
        
        // Only add valid tokens
        if (token.type != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    // Add EOF token
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", line));
    return tokens;
}

bool Tokenizer::isAtEnd() const {
    return current >= source.length();
}

char Tokenizer::advance() {
    return source[current++];
}

bool Tokenizer::match(char expected) {
    if (isAtEnd() || source[current] != expected) {
        return false;
    }
    
    current++;
    return true;
}

char Tokenizer::peek() const {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Tokenizer::peekNext() const {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

void Tokenizer::skipWhitespace() {
    while (true) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                line++;
                advance();
                break;
            case '-':
                if (peekNext() == '-') {
                    // Comment goes until the end of the line
                    while (peek() != '\n' && !isAtEnd()) {
                        advance();
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Tokenizer::scanToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", line);
    }
    
    char c = advance();
    
    // Handle identifiers
    if (isalpha(c) || c == '_') {
        while (isalnum(peek()) || peek() == '_') {
            advance();
        }
        
        std::string text = source.substr(start, current - start);
        std::string lowercase = text;
        for (char& c : lowercase) {
            c = std::tolower(c);
        }
        
        // Check if this is a keyword
        auto it = keywords.find(lowercase);
        if (it != keywords.end()) {
            return Token(it->second, text, line);
        }
        
        // Not a keyword, so it's an identifier
        return Token(TokenType::IDENTIFIER, text, line);
    }
    
    // Handle numbers
    if (isdigit(c)) {
        while (isdigit(peek())) {
            advance();
        }
        
        // Look for a decimal part
        if (peek() == '.' && isdigit(peekNext())) {
            // Consume the "."
            advance();
            
            while (isdigit(peek())) {
                advance();
            }
            
            return Token(TokenType::FLOAT_LITERAL, 
                         source.substr(start, current - start), line);
        }
        
        return Token(TokenType::INTEGER_LITERAL, 
                     source.substr(start, current - start), line);
    }
    
    // Handle strings
    if (c == '"' || c == '\'') {
        char quote = c;
        while (peek() != quote && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }
        
        if (isAtEnd()) {
            // Unterminated string
            return Token(TokenType::INVALID, 
                         "Unterminated string", line);
        }
        
        // Consume the closing quote
        advance();
        
        // Extract the string value (without the quotes)
        std::string value = source.substr(start + 1, current - start - 2);
        return Token(TokenType::STRING_LITERAL, value, line);
    }
    
    // Handle single-character tokens
    switch (c) {
        case '(': return Token(TokenType::LEFT_PAREN, "(", line);
        case ')': return Token(TokenType::RIGHT_PAREN, ")", line);
        case ',': return Token(TokenType::COMMA, ",", line);
        case ';': return Token(TokenType::SEMICOLON, ";", line);
        case '*': return Token(TokenType::STAR, "*", line);
        case '+': return Token(TokenType::PLUS, "+", line);
        case '-': return Token(TokenType::MINUS, "-", line);
        case '=': return Token(TokenType::EQUALS, "=", line);
        case '>': return Token(TokenType::GREATER, ">", line);
        case '<': return Token(TokenType::LESS, "<", line);
    }
    
    // If we got here, we encountered an unexpected character
    return Token(TokenType::INVALID, 
                 "Unexpected character: " + std::string(1, c), line);
}

std::string Token::toString() const {
    std::string typeStr;
    
    // Convert token type to string (simplified)
    switch (type) {
        case TokenType::SELECT: typeStr = "SELECT"; break;
        case TokenType::FROM: typeStr = "FROM"; break;
        case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case TokenType::STRING_LITERAL: typeStr = "STRING"; break;
        case TokenType::INTEGER_LITERAL: typeStr = "INTEGER"; break;
        case TokenType::EOF_TOKEN: typeStr = "EOF"; break;
        default: typeStr = "OTHER";
    }
    
    return typeStr + " " + lexeme;
}