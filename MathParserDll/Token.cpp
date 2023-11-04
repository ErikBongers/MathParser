#include "pch.hpp"
#include "Token.h"
#include "Number.h"


Token::Token(TokenType type, unsigned int len, SourcePos end)
    : type(type), range(end-len, end)
    {}

Token::Token(TokenType type, SourcePos pos, SourcePos end)
    : type(type), range(pos, end)
    {}

Token::Token(TokenType type, Number n, unsigned int len, SourcePos end)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), range(end-len, end)
    {}

Token::Token(TokenType type, Number n, SourcePos pos, SourcePos end)
    : type(type), numberValue(n), valueType(ValueType::NUMBER), range(pos, end)
    {}

