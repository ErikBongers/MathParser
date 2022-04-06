#include "pch.h"
#include "Tokenizer.h"
#include <algorithm>

Tokenizer::Tokenizer(const char* stream) 
    : _stream(stream) 
    { 
    size = strlen(stream); 
    _doPeek(); //assuming false for first peek.
    }

void Tokenizer::_doPeek()
    {
    peekedState.token = _nextToken();
    if(peekComments == false)
        { 
        //skip comments
        while(peekedState.token.type == TokenType::ECHO_COMMENT_LINE || peekedState.token.type == TokenType::COMMENT_LINE)
            peekedState.token = _nextToken();
        }
    peekedState.token.isFirstOnLine = peekedState.newLineStarted;
    peekedState.newLineStarted = false;
    }

Token Tokenizer::peek()
    {
    return peekedState.token;
    }

Token Tokenizer::peekSecond() 
    {
    State tmpState = peekedState;
    _doPeek();
    auto t = peekedState.token;
    peekedState = tmpState;
    return t;
    }

Token Tokenizer::next()
    {
    Token t = peekedState.token;
    _doPeek();
    return t;
    }

void Tokenizer::skipWhiteSpace()
    {
    char c;
    while ((c = peekChar()))
        {
        if (c != ' ' && c != '\n' && c != '\r')
            break;
        nextChar(); //consume
        }
    }

void Tokenizer::skipWhiteSpaceNoNL()
    {
    char c;
    while ((c = peekChar()))
        {
        if (c != ' ')
            break;
        nextChar(); //consume
        }
    }

Token Tokenizer::_nextToken()
    {
    if (peekedState.pos >= size)
        return Token(TokenType::EOT, getLine(), getLinePos());

    skipWhiteSpace();

    char c;
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
        case '|': return Token(TokenType::PIPE, c, getLine(), getLinePos());
        case '!': 
            {
            if (peekChar() == '/' && peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                auto comment = getToEOL();
                return Token(TokenType::ECHO_COMMENT_LINE, comment, getLine(), getLinePos());
                }
            else if (peekChar() == '/')
                {
                nextChar();
                return Token(TokenType::ECHO_END, "!/", getLine(), getLinePos()-1);
                }
            else if (peekChar() == '!')
                {
                nextChar();
                return Token(TokenType::ECHO_DOUBLE, "!", getLine(), getLinePos()-1);
                }
            return Token(TokenType::ECHO, c, getLine(), getLinePos());
            }
        case ';': 
            {
            auto t = Token(TokenType::SEMI_COLON, c, getLine(), getLinePos());
            return t;
            }
        case '.': 
            {
            if (peekChar() == '=')
                {
                nextChar(); //consume
                return Token(TokenType::EQ_UNIT, getLine(), getLinePos()-1);
                }
            else
                return Token(TokenType::DOT, c, getLine(), getLinePos());
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
            else if (peekString("define"))
                {
                getString("define");
                auto l = getLine();
                auto p = getPos();
                skipWhiteSpace();
                return Token(TokenType::DEFINE, getToEOL(), l, p);
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
                    {
                    nextChar(); //consume
                    auto comment = getToEOL();
                    return Token(TokenType::COMMENT_LINE, comment, getLine(), getLinePos());
                    }
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return _nextToken();
                case '#':
                    nextChar(); //consume
                    return Token(TokenType::MUTE_START, getLine(), getLinePos()-1);
                case '!':
                    nextChar(); //consume
                    return Token(TokenType::ECHO_START, getLine(), getLinePos()-1);
                }
            return Token(TokenType::DIV, c, getLine(), getLinePos());
            }
        case '\'':
            {
            auto start= peekedState.pos;
            auto curLine = peekedState.line;
            auto curLinePos = peekedState.linePos;
            skipToOnLine('\'');
            return Token(TokenType::QUOTED_STR, getText(start, std::max(start, peekedState.pos-1)), curLine, curLinePos);
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

    constexpr bool isIdChar(char c) 
        { 
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
            || c == '_'
            || (c >= '0' && c <= '9');
        }

    Token Tokenizer::parseId(char c)
        {
        std::string word = "";
        auto wordPos = getLinePos();
        word += c;

        while ((c = peekChar()))
            {
            if (isIdChar(c))
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

    char Tokenizer::nextChar()
        {
        if(peekedState.pos >= size)
            return 0; //EOF
        if(_stream[peekedState.pos] == '\n')
            {
            peekedState.line++;
            peekedState.linePos = 0;
            peekedState.newLineStarted = true;
            }
        else
            peekedState.linePos++;
        return _stream[peekedState.pos++];
        }

    char Tokenizer::peekChar()
        {
        if(peekedState.pos >= size)
            return 0; //EOF
        return _stream[peekedState.pos];
        }

    char Tokenizer::peekSecondChar()
        {
        if((peekedState.pos+1) >= size)
        return 0; //EOF
    return _stream[peekedState.pos+1];
        }


    Number Tokenizer::parseDecimal(char c)
        {
        //we already have the first digit
        double d = 0;
        int e = 0;
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
            else if (c == '_')
                {
                nextChar();
                continue;
                }
            else
                {
                break;
                }
            }
        if (peekChar() == 'e' || peekChar() == 'E')
            {
            nextChar();//consume 'E'
            e = parseInteger();
            }
        return Number(d, e);
        }

    int Tokenizer::parseInteger()
        {
        int i = 0;
        char c;
        int factor = 1;
        if (peekChar() == '-')
            {
            nextChar();
            factor = -1;
            }
        else if (peekChar() == '+')
            {
            nextChar();
            }

        while ((c = peekChar()))
            {
            if (c >= '0' && c <= '9')
                {
                nextChar(); //consume
                i = i * 10 + (c - '0');
                }
            else if (c == '_')
                {
                nextChar();
                continue;
                }
            else
                {
                break;
                }
            }
        return i*factor;
        }

    double Tokenizer::parseBinary()
        {
        nextChar(); //consume 'b'
        unsigned long bin = 0;

        while(peekChar() == '0' || peekChar() == '1' || peekChar() == '_')
            {
            char c = nextChar();
            if(c == '_')
                continue;

            bin <<=1;
            if(c == '1')
                bin++;
            }
        return bin;
        }

    double Tokenizer::parseHex()
        {
        nextChar(); //consume 'x'
        unsigned long hex = 0;

        while((peekChar() >= '0' && peekChar() <= '9') 
              || (peekChar() >= 'a' && peekChar() <= 'f') 
              || (peekChar() >= 'A' && peekChar() <= 'F') 
              || peekChar() == '_')
            {
            char c = nextChar();
            if(c == '_')
                continue;

            hex *=16;
            if(c >= '0' && c <= '9')
                hex += c - '0';
            else if( (c >= 'a' && c <= 'f'))
                hex += 10 + c - 'a';
            else if( (c >= 'A' && c <= 'F'))
                hex += 10 + c - 'A';
            }
        return hex;
        }

    Token Tokenizer::parseNumber(char c)
        {
        auto numPos = getLinePos();

        if(peekChar() == 'b' || peekChar() == 'B')
            return Token(TokenType::NUMBER, Number(parseBinary(), 0, NumFormat::BIN), getLine(), numPos);
        else if(peekChar() == 'x' || peekChar() == 'X')
            return Token(TokenType::NUMBER, Number(parseHex(), 0, NumFormat::HEX), getLine(), numPos);
        else
            return Token(TokenType::NUMBER, parseDecimal(c), getLine(), numPos);
        }

    std::string Tokenizer::getToEOL()
        {
        std::string buf;
        char c;
        while ((c = peekChar())) 
            {
            if(c == '\n')
                break; //don't eat newLine yet. The nextToken should be marked as firstOnNewLine.
            nextChar();
            if(c != '\r')
                buf += c;
            }
        return buf;
        }

    void Tokenizer::skipToEOL()
        {
        char c;
        while ((c = nextChar()))
            {
            if(c == '\n')
                break;
            }
        }

    void Tokenizer::skipToOnLine(char c)
        {
        char cc;
        while ((cc = nextChar()))
            {
            if(cc == '\n')
                break;
            if(c == cc)
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

    bool Tokenizer::peekString(std::string str)
        {
        if(peekedState.pos + str.size() >= size)
        return false;
    if(str.compare(0, str.size(), _stream, peekedState.pos, str.size()) != 0)
        return false;
    if(peekedState.pos + str.size() == size)
        return true;//EOF, so ID-string matches.
                    //next char should not be an ID char
    return !isIdChar(_stream[peekedState.pos+str.size()]);
    }

bool Tokenizer::getString(std::string str)
    {
    if(!peekString(str))
        return false;
    for(int i = 0; i < str.size(); i++)
        nextChar();//this function does some administration, so we can't just set this->pos.
    return true;
    }

