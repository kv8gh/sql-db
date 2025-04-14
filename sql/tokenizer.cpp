#include "./tokenizer.hpp"
#include <cctype>
#include <unordered_map>
#include <iostream>

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
    // Add this at the start of the function
    std::cout << "Scanning token at position " << current << std::endl;
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
        std:: cout<< "This is the lowercase " << lowercase<< std::endl;
        auto it = keywords.find(lowercase);
        if (it != keywords.end()) {
            std::cout << "Found keyword: " << text << " -> TokenType: " << static_cast<int>(it->second) << std::endl;
            return Token(it->second, text, line);
        }
        
        // Not a keyword, so it's an identifier
        std::cout << "Found identifier: " << text << std::endl;
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
    
    // Convert token type to string (add all cases)
    switch (type) {
        case TokenType::SELECT: typeStr = "SELECT"; break;
        case TokenType::INSERT: typeStr = "INSERT"; break;
        case TokenType::UPDATE: typeStr = "UPDATE"; break;
        case TokenType::DELETE: typeStr = "DELETE"; break;
        case TokenType::CREATE: typeStr = "CREATE"; break;
        case TokenType::DROP: typeStr = "DROP"; break;
        case TokenType::TABLE: typeStr = "TABLE"; break;
        case TokenType::FROM: typeStr = "FROM"; break;
        case TokenType::WHERE: typeStr = "WHERE"; break;
        case TokenType::INTO: typeStr = "INTO"; break;
        case TokenType::VALUES: typeStr = "VALUES"; break;
        case TokenType::SET: typeStr = "SET"; break;
        case TokenType::INTEGER: typeStr = "INTEGER"; break;
        case TokenType::TEXT: typeStr = "TEXT"; break;
        case TokenType::REAL: typeStr = "REAL"; break;
        case TokenType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
        case TokenType::STRING_LITERAL: typeStr = "STRING"; break;
        case TokenType::INTEGER_LITERAL: typeStr = "INTEGER_LIT"; break;
        case TokenType::FLOAT_LITERAL: typeStr = "FLOAT_LIT"; break;
        case TokenType::LEFT_PAREN: typeStr = "LEFT_PAREN"; break;
        case TokenType::RIGHT_PAREN: typeStr = "RIGHT_PAREN"; break;
        case TokenType::COMMA: typeStr = "COMMA"; break;
        case TokenType::SEMICOLON: typeStr = "SEMICOLON"; break;
        case TokenType::STAR: typeStr = "STAR"; break;
        case TokenType::PLUS: typeStr = "PLUS"; break;
        case TokenType::MINUS: typeStr = "MINUS"; break;
        case TokenType::EQUALS: typeStr = "EQUALS"; break;
        case TokenType::GREATER: typeStr = "GREATER"; break;
        case TokenType::LESS: typeStr = "LESS"; break;
        case TokenType::EOF_TOKEN: typeStr = "EOF"; break;
        default: typeStr = "OTHER";
    }
    
    return typeStr + " " + lexeme;
}