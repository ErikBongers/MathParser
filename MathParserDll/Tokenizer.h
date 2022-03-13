#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Number.h"

enum class NumFormat { DEC, BIN, HEX };

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
    EQ_UNIT,
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
    ECHO_START,
    ECHO_END,
    MUTE_LINE,
    MUTE_START,
    MUTE_END,

    UNKNOWN,
    EOT,
    NULLPTR
    };

class Token
    {
    public:
        static Token Null() { return Token(TokenType::NULLPTR, 0, 0); }
        bool isNull() const { return type == TokenType::NULLPTR;}
        TokenType type;
        std::string stringValue;
        Number numberValue = {0,0};
        NumFormat numFormat = NumFormat::DEC;
        unsigned int pos;
        unsigned int line;
        bool isFirstOnLine = false;
        Token() : Token(TokenType::NULLPTR, 0, 0) {}
        Token(TokenType type, char c, unsigned int line, unsigned int pos) 
            : line(line), pos(pos)
            {
            this->type = type;
            this->stringValue += c;
            }
        Token(TokenType type, Number n, unsigned int line, unsigned int pos, NumFormat numFormat = NumFormat::DEC)
            : line(line), pos(pos), numFormat(numFormat)
            {
            this->type = type;
            this->numberValue = n;
            }
        Token(TokenType type, std::string str, unsigned int line, unsigned int pos)
            : line(line), pos(pos)
            {
            this->type = type;
            this->stringValue = str;
            }
        Token(TokenType type, unsigned int line, unsigned int pos)
            : Token(type, Number(0.0, 0), line, pos) 
            { }
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
        Token peek(bool includeEchoComment = false);
        Token next(bool includeEchoComment = false);
        unsigned int getLine() { return line;}
        unsigned int getLinePos() { return linePos-1;} //linePos always contains the NEXT pos.
        unsigned int getPos() { return pos;} //pos always contains the NEXT pos.
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
        void skipWhiteSpace();

    private:
        char nextChar();
        inline char peekChar();
        inline char peekSecondChar();
        Token parseId(char c);

        Number parseDecimal(char c);
        double parseBinary();
        double parseHex();
        int parseInteger();

        Token parseNumber(char c);

        void skipToEOL();
        std::string getToEOL();
        void skipToEndOfComment();
        Token _nextToken();
    };

