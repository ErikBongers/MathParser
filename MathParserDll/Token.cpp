#include "pch.hpp"
#include "Token.h"
#include "Number.h"


Token::Token(TokenType type, char c, TokenPos pos)
    : type(type), stringValue(1, c), pos(pos)
    {}

Token::Token(TokenType type, Number n, TokenPos pos)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), pos(pos)
    {}

Token::Token(TokenType type, std::string str, TokenPos pos)
    : type(type), stringValue(str), pos(pos)
    {}

Token::Token(TokenType type, TokenPos pos)
    : Token(type, Number(0.0, 0), pos)
    {}

