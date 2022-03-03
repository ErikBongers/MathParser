#pragma once
#include <string>
#include <map>
#include <vector>
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
    INC,
    DEC,
    EQ,
    EQ_PLUS,
    EQ_MIN,
    EQ_MULT,
    EQ_DIV,
    NUMBER,
    POWER,
    ID,
    SEMI_COLON,
    COMMA,
    DOT, //except for the decimal dot.
    QUOTE,
    PIPE,
    ECHO,
    ECHO_COMMENT_LINE,
    MUTE_LINE,
    MUTE_START,
    MUTE_END,

    UNKNOWN,
    EOT,
    NULLPTR
    };

const char* to_string(TokenType tt);

class Token
    {
    public:
        static Token Null() { return Token(TokenType::NULLPTR, 0, 0); }
        bool isNull() const { return type == TokenType::NULLPTR;}
        TokenType type;
        std::string stringValue;
        double numberValue;
        unsigned int pos;
        unsigned int line;
        bool isFirstOnLine = false;
        Token() : Token(TokenType::NULLPTR, 0, 0) {}
        Token(TokenType type, char c, unsigned int line, unsigned int pos) 
            : numberValue(-1), line(line), pos(pos)
            {
            this->type = type;
            this->stringValue += c;
            }
        Token(TokenType type, double d, unsigned int line, unsigned int pos)
            : line(line), pos(pos)
            {
            this->type = type;
            this->numberValue = d;
            }
        Token(TokenType type, std::string str, unsigned int line, unsigned int pos)
            : line(line), pos(pos)
            {
            this->type = type;
            this->stringValue = str;
            }
        Token(TokenType type, unsigned int line, unsigned int pos)
            : Token(type, 0.0, line, pos) 
            { }
        const std::string to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Token& t);
    };


class Tokenizer
    {
    private:
        const char* _stream;
        unsigned int pos = 0; //pos at which to read next token.
        unsigned int line = 0;
        unsigned int linePos = 0;
        bool newLineStarted = true;
        size_t size = -1;
    public:
        Tokenizer(const char* stream) : _stream(stream) { size = strlen(stream); }
        Token peek();
        Token next();
        unsigned int getLine() { return line;}
        unsigned int getLinePos() { return linePos-1;} //linePos always contains the NEXT pos.
        unsigned int getPos() { return pos;} //pos always contains the NEXT pos.
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }

    private:
        char nextChar();
        char peekChar();
        char peekSecondChar();
        Token parseId(char c);

        Token parseNumber(char c);

        void skipToEOL();
        std::string getToEOL();
        void skipToEndOfComment();
        Token _nextToken();
    };

