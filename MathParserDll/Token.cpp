#include "pch.hpp"
#include "Token.h"
#include "Number.h"


Token::Token(TokenType type, char c, TokenPos pos, char sourceIndex)
    : type(type), stringValue(1, c), pos(pos), sourceIndex(sourceIndex)
    {}

Token::Token(TokenType type, Number n, TokenPos pos, char sourceIndex)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), pos(pos), sourceIndex(sourceIndex)
    {}

Token::Token(TokenType type, std::string str, TokenPos pos, char sourceIndex)
    : type(type), stringValue(str), pos(pos), sourceIndex(sourceIndex)
    {}

Token::Token(TokenType type, TokenPos pos, char sourceIndex)
    : Token(type, Number(0.0, 0, Range()), pos, sourceIndex)
    {}

