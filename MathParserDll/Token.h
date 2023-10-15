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

    //keep these together!
    EQ,
    EQ_PLUS,
    EQ_MIN,
    EQ_MULT,
    EQ_DIV,
    EQ_UNIT,
    //keep above together!

    NUMBER,
    POWER,
    ID,
    SEMI_COLON,
    COMMA,
    DOT, //except for the decimal dot.
    ELLIPSIS, //...
    PIPE,
    EXCLAM,
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
    PERCENT,
    MODULO,

    DEFINE,
    UNDEF,

    UNKNOWN,
    EOT,
    NULLPTR
    };

class Token
    {
    public:
        static Token Null() { return Token(TokenType::NULLPTR, 0, {0, 0, 0, -1}); }
        bool isNull() const { return type == TokenType::NULLPTR;}
        TokenType type;
        Range range;
        ValueType valueType = ValueType::NONE;
        Number numberValue = Number(0.0,0, Range());

        bool isFirstOnLine = false;
        Token() : Token(TokenType::NULLPTR, -1, {0, 0, 0}) {}
        Token(TokenType type, unsigned int len, TokenPos end);
        Token(TokenType type, Number n, unsigned int len, TokenPos end);
        Token(TokenType type, Number n, TokenPos pos, TokenPos end);
        Token(TokenType type, TokenPos pos, TokenPos end);
    };


