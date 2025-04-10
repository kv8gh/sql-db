#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <string>
#include <vector>
#include "./token.hpp"

class Tokenizer {
public:
    Tokenizer(const std::string& source);
    
    // Scan all tokens from the source
    std::vector<Token> scanTokens();
    
private:
    const std::string& source;
    size_t start;      // Start of the current lexeme
    size_t current;    // Current position in the source
    int line;          // Current line in the source
    
    // Helper methods
    bool isAtEnd() const;
    char advance();
    bool match(char expected);
    char peek() const;
    char peekNext() const;
    void skipWhitespace();
    Token scanToken();
};

#endif // TOKENIZER_HPP