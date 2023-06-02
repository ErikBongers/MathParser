#include "pch.hpp"
#include "Token.h"
#include "Number.h"


Token::Token(TokenType type, unsigned int len, TokenPos end)
    : type(type), range(end-len, end)
    {}

Token::Token(TokenType type, TokenPos pos, TokenPos end)
    : type(type), range(pos, end)
    {}

Token::Token(TokenType type, Number n, unsigned int len, TokenPos end)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), range(end-len, end)
    {}

Token::Token(TokenType type, Number n, TokenPos pos, TokenPos end)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), range(pos, end)
    {}

