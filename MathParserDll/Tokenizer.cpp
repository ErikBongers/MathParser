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
        { TokenType::ECHO, "!" },
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
    while (char c = peekChar())
        {
        if (c != ' ' && c != '\n' && c != '\r')
            break;
        nextChar(); //consume
        }


    char c = nextChar();
    if (!c)
        return Token(TokenType::EOT);

    switch (c)
        {
        case '!': return Token(TokenType::ECHO, c);
        case '{': return Token(TokenType::CURL_OPEN, c);
        case '}': return Token(TokenType::CURL_CLOSE, c);
        case '(': return Token(TokenType::PAR_OPEN, c);
        case ')': return Token(TokenType::PAR_CLOSE, c);
        case '[': return Token(TokenType::BRAC_OPEN, c);
        case ']': return Token(TokenType::BRAC_CLOSE, c);
        case '^': return Token(TokenType::POWER, c);
        case '=': return Token(TokenType::EQ, c);
        case ';': return Token(TokenType::SEMI_COLON, c);
        case ',': return Token(TokenType::COMMA, c);
        case '.': return Token(TokenType::DOT, c);
        case '\'': return Token(TokenType::QUOTE, c);
        case '|': return Token(TokenType::PIPE, c);

        case '+':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_PLUS);
                }
            else if (peekChar() == '+')
                {
                nextChar(); //consume
                return Token(TokenType::INC);
                }
            return Token(TokenType::PLUS, c);
            }
        case '-':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_MIN);
                }
            else if (peekChar() == '-')
                {
                nextChar(); //consume
                return Token(TokenType::DEC);
                }
            return Token(TokenType::MIN, c);
            }
        case '*':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_MULT);
                }
            return Token(TokenType::MULT, c);
            }
        case '/':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_DIV);
                }

            if (peekChar() == '/')
                {
                nextChar(); //consume
                skipToEOL();
                return next();
                }
            else if (peekChar() == '*')
                {
                nextChar(); //consume
                skipToEndOfComment();
                return next();
                }
            return Token(TokenType::DIV, c);
            }
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

    while (c = peekChar())
        {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || c == '_'
            || (c >= '0' && c <= '9')
            )
            {
            nextChar(); //consume
            word += c;
            }
        else
            {
            break;
            }
        }
    std::string lower;
    lower.resize(word.length());
    std::transform(word.begin(), word.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });
    return Token(TokenType::ID, word);
    }

char Tokenizer::nextChar()
    {
    if(pos >= size)
        return 0; //EOF
    return _stream[pos++];
    }

char Tokenizer::peekChar()
    {
    if(pos >= size)
        return 0; //EOF
    return _stream[pos];
    }

char Tokenizer::peekSecondChar()
    {
    if((pos+1) >= size)
        return 0; //EOF
    return _stream[pos+1];
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

    while (c = peekChar())
        {
        if (c >= '0' && c <= '9')
            {
            nextChar(); //consume
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
            char cc = peekSecondChar();
            if (cc >= '0' && cc <= '9')
                {
                nextChar(); //consume DOT
                decimalDivider = 10;
                }
            else//dot has other meaning
                {
                break;
                }
            }
        else
            {
            break;
            }
        }

    return Token(TokenType::NUMBER, d);
    }

void Tokenizer::skipToEOL()
    {
    while (char c = nextChar())
        {
        if(c == '\n')
            break;
        }
    }

void Tokenizer::skipToEndOfComment()
    {
    while(true)
        {
        while (char c = nextChar())
            {
            if(c == '*')
                break;
            }
        if (peekChar() == '/')
            {
            nextChar(); //consume
            return;
            }
        if(!peekChar())
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
