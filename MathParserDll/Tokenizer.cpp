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
        peekedToken = Token::Null();
        return t;
        }
    if (pos >= size)
        return Token(TokenType::EOT, getLine(), getLinePos());

    //skip whitespace
    char c;
    while ((c = peekChar()))
        {
        if (c != ' ' && c != '\n' && c != '\r')
            break;
        nextChar(); //consume
        }


    c = nextChar();
    if (!c)
        return Token(TokenType::EOT, getLine(), getLinePos());

    switch (c)
        {
        case '{': return Token(TokenType::CURL_OPEN, c, getLine(), getLinePos());
        case '}': return Token(TokenType::CURL_CLOSE, c, getLine(), getLinePos());
        case '(': return Token(TokenType::PAR_OPEN, c, getLine(), getLinePos());
        case ')': return Token(TokenType::PAR_CLOSE, c, getLine(), getLinePos());
        case '[': return Token(TokenType::BRAC_OPEN, c, getLine(), getLinePos());
        case ']': return Token(TokenType::BRAC_CLOSE, c, getLine(), getLinePos());
        case '^': return Token(TokenType::POWER, c, getLine(), getLinePos());
        case '=': return Token(TokenType::EQ, c, getLine(), getLinePos());
        case ',': return Token(TokenType::COMMA, c, getLine(), getLinePos());
        case '.': return Token(TokenType::DOT, c, getLine(), getLinePos());
        case '\'': return Token(TokenType::QUOTE, c, getLine(), getLinePos());
        case '|': return Token(TokenType::PIPE, c, getLine(), getLinePos());
        case '!': 
            {
            if (peekChar() == '/' && peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                skipToEOL(true);
                comment_lines.push_back(currentStatement);
                currentStatement.clear();
                return next();
                }
            return Token(TokenType::ECHO, c, getLine(), getLinePos());
            }
        case ';': 
            {
            auto t = Token(TokenType::SEMI_COLON, currentStatement, getLine(), getLinePos());
            currentStatement.clear();
            return t;
            }
        case '+':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_PLUS, getLine(), getLinePos()-1);
                }
            else if (peekChar() == '+')
                {
                nextChar(); //consume
                return Token(TokenType::INC, getLine(), getLinePos()-1);
                }
            return Token(TokenType::PLUS, c, getLine(), getLinePos());
            }
        case '-':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_MIN, getLine(), getLinePos()-1);
                }
            else if (peekChar() == '-')
                {
                nextChar(); //consume
                return Token(TokenType::DEC, getLine(), getLinePos()-1);
                }
            return Token(TokenType::MIN, c, getLine(), getLinePos());
            }
        case '*':
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_MULT, getLine(), getLinePos()-1);
                }
            return Token(TokenType::MULT, c, getLine(), getLinePos());
            }
        case '#':
            if (peekChar() == '/')
                {
                nextChar(); //consume
                return Token(TokenType::MUTE_END, getLine(), getLinePos()-1);
                }
            return Token(TokenType::MUTE_LINE, c, getLine(), getLinePos());
        case '/':
            {
            auto cc = peekChar();
            switch (cc)
                {
                case '=':
                    nextChar(); //consume
                    return Token(TokenType::EQ_DIV, getLine(), getLinePos()-1);
                case '/':
                    nextChar(false); //consume
                    currentStatement.pop_back(); //remove the first '/' from the statement.
                    skipToEOL(false);
                    return next();
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return next();
                case '#':
                    nextChar(); //consume
                    return Token(TokenType::MUTE_START, getLine(), getLinePos()-1);
                }
            return Token(TokenType::DIV, c, getLine(), getLinePos());
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
                return Token(TokenType::UNKNOWN, c, getLine(), getLinePos());
        }
    }

Token Tokenizer::parseId(char c)
    {
    std::string word = "";
    auto wordPos = getLinePos();
    word += c;

    while ((c = peekChar()))
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
    return Token(TokenType::ID, word, getLine(), wordPos);
    }

char Tokenizer::nextChar(bool storeChars)
    {
    if(pos >= size)
        return 0; //EOF
    if(storeChars)
        currentStatement.push_back(_stream[pos]);
    if(_stream[pos] == '\n')
        {
        line++;
        linePos = 0;
        }
    linePos++;
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
    auto numPos = getLinePos();
    //we already have the first digit
    double d = 0;
    double decimalDivider = 1;
    if (c == '.')
        decimalDivider = 10;
    else
        d = c - '0';

    while ((c = peekChar()))
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

    return Token(TokenType::NUMBER, d, getLine(), numPos);
    }

void Tokenizer::skipToEOL(bool storeChars)
    {
    char c;
    while ((c = nextChar(storeChars)))
        {
        if(c == '\n')
            break;
        }
    }

void Tokenizer::skipToEndOfComment()
    {
    while(true)
        {
        char c;
        while ((c = nextChar()))
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
