#pragma once
#include "ValueType.h"
#include "Number.h"
#include "TokenPos.h"

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
    FUNCTION,

    DEFINE,
    UNDEF,

    UNKNOWN,
    EOT,
    NULLPTR
    };

class Token
    {
    public:
        static Token Null(char sourceIndex) { return Token(TokenType::NULLPTR, {0, 0, 0}, sourceIndex); }
        bool isNull() const { return type == TokenType::NULLPTR;}
        TokenType type;
        std::string stringValue;
        char sourceIndex = -1;

        ValueType valueType = ValueType::NONE;
        Number numberValue = Number(0.0,0, Range());
        //Timepoint
        //Duration
        //Calendar

        TokenPos pos;
        bool isFirstOnLine = false;
        Token() : Token(TokenType::NULLPTR, {0, 0, 0}, -1) {}
        Token(TokenType type, char c, TokenPos pos, char sourceIndex);
        Token(TokenType type, Number n, TokenPos pos, char sourceIndex);
        Token(TokenType type, std::string str, TokenPos pos, char sourceIndex);
        Token(TokenType type, TokenPos pos, char sourceIndex);
    };


