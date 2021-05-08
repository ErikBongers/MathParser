#include "pch.h"
#include "Tokenizer.h"
#include <algorithm>

std::ostream& operator<<(std::ostream& os, const Token& t) 
    { 
    os << t.to_string(); return os; 
    }

const char* to_string(TokenType tt)
    {
    const std::map<TokenType, const char*> TokenTypeStringMap{
        { TokenType::BRAC_OPEN, "BRAC_OPEN" },
        { TokenType::BRAC_CLOSE, "BRAC_CLOSE" },
        { TokenType::CURL_OPEN, "CURL_OPEN" },
        { TokenType::CURL_CLOSE, "CURL_CLOSE" },
        { TokenType::PAR_OPEN, "PAR_OPEN" },
        { TokenType::PAR_CLOSE, "PAR_CLOSE" },
        { TokenType::PLUS, "PLUS" },
        { TokenType::MIN, "MIN" },
        { TokenType::DIV, "DIV" },
        { TokenType::MULT, "MULT" },
        { TokenType::EQ, "EQ" },
        { TokenType::NUMBER, "NUMBER" },
        { TokenType::POWER, "POWER" },
        { TokenType::ID, "ID" },
        { TokenType::RAD, "RAD" },
        { TokenType::DEG, "DEG" },
        { TokenType::SEMI_COLON, "SEMI_COLON" },
        { TokenType::UNKNOWN, "EOT" }
        };
    auto   it = TokenTypeStringMap.find(tt);
    return it == TokenTypeStringMap.end() ? "??" : it->second;
    }

Token Tokenizer::peek()
    {
    if (peekedToken.type != TokenType::NULLPTR)
        return peekedToken;
    peekedToken = next();
    return peekedToken;
    }

Token Tokenizer::next()
    {
    if (peekedToken.type != TokenType::NULLPTR)
        {
        Token t = peekedToken;
        peekedToken = Token(TokenType::NULLPTR);
        return t;
        }
    if (pos >= strlen(stream))
        return Token(TokenType::EOT);

    //skip whitespace
    while (pos < strlen(stream))
        {
        if (stream[pos] != ' ' && stream[pos] != '\n' && stream[pos] != '\r')
            break;
        pos++;
        }

    if (pos >= strlen(stream))
        return Token(TokenType::EOT);

    char c = stream[pos];
    pos++;
    switch (c)
        {
        case '{':
            return Token(TokenType::CURL_OPEN, c);
        case '}':
            return Token(TokenType::CURL_CLOSE, c);
        case '(':
            return Token(TokenType::PAR_OPEN, c);
        case ')':
            return Token(TokenType::PAR_CLOSE, c);
        case '[':
            return Token(TokenType::BRAC_OPEN, c);
        case ']':
            return Token(TokenType::BRAC_CLOSE, c);

        case '+':
            return Token(TokenType::PLUS, c);
        case '-':
            return Token(TokenType::MIN, c);
        case '*':
            return Token(TokenType::MULT, c);
        case '/':
            return Token(TokenType::DIV, c);
        case '^':
            return Token(TokenType::POWER, c);
        case '=':
            return Token(TokenType::EQ, c);
        case ';':
            return Token(TokenType::SEMI_COLON, c);
        default:
            if ((c >= '0' && c <= '9') || c == '.')
                {
                return parseNumber(c);
                }
            else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                {
                return parseId(c);
                }
            else
                return Token(TokenType::UNKNOWN, c);
        }
    }

Token Tokenizer::parseId(char c)
    {
    std::string word = "";

    word += c;

    while (pos < strlen(stream))
        {
        c = stream[pos];
        pos++;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')
            {
            word += c;
            }
        else
            {
            pos--;
            break;
            }
        }
    std::string lower;
    lower.resize(word.length());
    std::transform(word.begin(), word.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });
    if (lower.compare("deg") == 0)
        return Token(TokenType::DEG, word);
    else if (lower.compare("rad") == 0)
        return Token(TokenType::RAD, word);
    else
        return Token(TokenType::ID, word);
    }

Token Tokenizer::parseNumber(char c)
    {
    //we already have the first digit
    double d = 0;
    double decimalDivider = 1;
    if (c == '.')
        decimalDivider = 10;
    else
        d = c - '0';

    while (pos < strlen(stream))
        {
        c = stream[pos];
        pos++;
        if (c >= '0' && c <= '9')
            {
            if (decimalDivider == 1)
                d = d * 10 + (c - '0');
            else
                {
                d += (c - '0') / decimalDivider;
                decimalDivider *= 10;
                }
            }
        else if (c == '.')
            {
            decimalDivider = 10;
            }
        else
            {
            pos--;
            break;
            }
        }

    return Token(TokenType::NUMBER, d);
    }

const std::string Token::to_string() const
    {
    if (type == TokenType::NUMBER)
        return std::to_string(numberValue);
    else if (type == TokenType::ID)
        return stringValue;
    else
        return ::to_string(type);
    }
