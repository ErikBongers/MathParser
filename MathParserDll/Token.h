#pragma once
#include "ValueType.h"
#include "Number.h"

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
    PIPE,
    ECHO,
    ECHO_COMMENT_LINE,
    COMMENT_LINE,
    ECHO_START,
    ECHO_END,
    ECHO_DOUBLE,
    MUTE_LINE,
    MUTE_START,
    MUTE_END,
    QUOTED_STR,
    NEWLINE,

    DEFINE,
    UNDEF,

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

        ValueType valueType = ValueType::NONE;
        Number numberValue = Number(0.0,0);
        //Timepoint
        //Duration
        //Calendar

        unsigned int pos;
        unsigned int line;
        bool isFirstOnLine = false;
        Token() : Token(TokenType::NULLPTR, 0, 0) {}
        Token(TokenType type, char c, unsigned int line, unsigned int pos);
        Token(TokenType type, Number n, unsigned int line, unsigned int pos);
        Token(TokenType type, std::string str, unsigned int line, unsigned int pos);
        Token(TokenType type, unsigned int line, unsigned int pos);
    };


