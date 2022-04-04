#include "pch.h"
#include "Token.h"
#include "Number.h"


Token::Token(TokenType type, char c, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), stringValue(1, c)
    {}

Token::Token(TokenType type, Number n, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), numberValue(n), valueType(ValueType::NUMBER)
    {}

Token::Token(TokenType type, std::string str, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(type), stringValue(str)
    {}

Token::Token(TokenType type, unsigned int line, unsigned int pos)
    : Token(type, Number(0.0, 0), line, pos)
    {}

