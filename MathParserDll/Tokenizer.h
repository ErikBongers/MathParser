#pragma once
#include <string>
#include <map>
#include <iostream>

enum class TokenType
    {
    BRAC_OPEN,
    BRAC_CLOSE,
    CURL_OPEN,
    CURL_CLOSE,
    PAR_OPEN,
    PAR_CLOSE,
    PLUS,
    MIN,
    DIV,
    MULT,
    EQ,
    NUMBER,
    POWER,
    ID,
    SEMI_COLON,
    COMMA,
    DOT, //except for the decimal dot.
    QUOTE,

    UNKNOWN,
    EOT,
    NULLPTR
    };

const char* to_string(TokenType tt);

class Token
    {
    public:
        TokenType type;
        std::string stringValue;
        double numberValue;
        Token() : Token(TokenType::NULLPTR) {}
        Token(TokenType type, char c) : numberValue(-1)
            {
            this->type = type;
            this->stringValue += c;
            }
        Token(TokenType type, double d)
            {
            this->type = type;
            this->numberValue = d;
            }
        Token(TokenType type, std::string str)
            {
            this->type = type;
            this->stringValue = str;
            }
        Token(TokenType type) : Token(type, ' ') { }
        const std::string to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Token& t);
    };


class Tokenizer
    {
    private:
        const char* stream;
        int pos = 0; //pos at which to read next token.
        Token peekedToken = Token(TokenType::NULLPTR);
        size_t size = -1;
    public:
        Tokenizer(const char* stream) : stream(stream) { size = strlen(stream); }
        Token peek();
        Token next();

    private:
        Token parseId(char c);

        Token parseNumber(char c);

        void skipToEOL();
    };

