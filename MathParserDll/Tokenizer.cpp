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
        { TokenType::INC, "INC" },
        { TokenType::DEC, "DEC" },
        { TokenType::EQ, "EQ" },
        { TokenType::NUMBER, "NUMBER" },
        { TokenType::POWER, "POWER" },
        { TokenType::ID, "ID" },
        { TokenType::SEMI_COLON, "SEMI_COLON" },
        { TokenType::COMMA, "COMMA" },
        { TokenType::PIPE, "PIPE" },
        { TokenType::UNKNOWN, "??" },
        { TokenType::NULLPTR, "" }
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
    if (pos >= size)
        return Token(TokenType::EOT);

    //skip whitespace
    while (pos < size)
        {
        if (stream[pos] != ' ' && stream[pos] != '\n' && stream[pos] != '\r')
            break;
        pos++;
        }

    if (pos >= size)
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
            {
            if (pos < size && stream[pos] == '=')
                {
                pos++;
                return Token(TokenType::EQ_PLUS);
                }
            else if (pos < size && stream[pos] == '+')
                {
                pos++;
                return Token(TokenType::INC);
                }
            return Token(TokenType::PLUS, c);
            }
        case '-':
            {
            if (pos < size && stream[pos] == '=')
                {
                pos++;
                return Token(TokenType::EQ_MIN);
                }
            else if (pos < size && stream[pos] == '-')
                {
                pos++;
                return Token(TokenType::DEC);
                }
            return Token(TokenType::MIN, c);
            }
        case '*':
            {
            if (pos < size && stream[pos] == '=')
                {
                pos++;
                return Token(TokenType::EQ_MULT);
                }
            return Token(TokenType::MULT, c);
            }
        case '/':
            {
            if (pos < size && stream[pos] == '=')
                {
                pos++;
                return Token(TokenType::EQ_DIV);
                }

            if (stream[pos] == '/')
                {
                pos++;
                skipToEOL();
                return next();
                }
            else if (stream[pos] == '*')
                {
                pos++;
                skipToEndOfComment();
                return next();
                }
            return Token(TokenType::DIV, c);
            }
        case '^':
            return Token(TokenType::POWER, c);
        case '=':
            return Token(TokenType::EQ, c);
        case ';':
            return Token(TokenType::SEMI_COLON, c);
        case ',':
            return Token(TokenType::COMMA, c);
        case '.':
            return Token(TokenType::DOT, c);
        case '\'':
            return Token(TokenType::QUOTE, c);
        case '|':
            return Token(TokenType::PIPE, c);
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

    while (pos < size)
        {
        c = stream[pos];
        pos++;
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || c == '_'
            || (c >= '0' && c <= '9')
            )
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

    while (pos < size)
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
            if (stream[pos] >= '0' && stream[pos] <= '9')
                decimalDivider = 10;
            else//dot has other meaning
                {
                pos--;
                break;
                }
            }
        else
            {
            pos--;
            break;
            }
        }

    return Token(TokenType::NUMBER, d);
    }

void Tokenizer::skipToEOL()
    {
    while (stream[pos] != '\n' && pos < size)
        pos++;
    pos++;
    }

void Tokenizer::skipToEndOfComment()
    {
    while(true)
        {
        while (stream[pos] != '*' && pos < size)
            {
            pos++;
            }
        pos++;
        if (stream[pos] == '/')
            {
            pos++;
            return;
            }
        if(pos >= size)
            return;
        }
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
