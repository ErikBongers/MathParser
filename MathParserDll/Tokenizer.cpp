#include "pch.h"
#include "Tokenizer.h"
#include <algorithm>

Tokenizer::Tokenizer(const char* stream) 
    : BaseTokenizer(stream) 
    { 
    getNextState();
    }

Token Tokenizer::peekSecond() 
    {
    State tmpState = peekedState;
    getNextState();
    auto t = peekedState.token;
    peekedState = tmpState;
    return t;
    }

Token Tokenizer::next()
    {
    Token t = peekedState.token;
    getNextState();
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

void Tokenizer::getNextState()
    {
    peekedState.token = getNextToken();
    if(peekComments == false)
        { 
        //skip comments
        while(peekedState.token.type == TokenType::ECHO_COMMENT_LINE || peekedState.token.type == TokenType::COMMENT_LINE)
            peekedState.token = getNextToken();
        }
    peekedState.token.isFirstOnLine = peekedState.newLineStarted;
    peekedState.newLineStarted = false;
    }

Token Tokenizer::getNextToken()
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
        using enum TokenType;
        case '{': return Token(CURL_OPEN, c, getLine(), getLinePos());
        case '}': return Token(CURL_CLOSE, c, getLine(), getLinePos());
        case '(': return Token(PAR_OPEN, c, getLine(), getLinePos());
        case ')': return Token(PAR_CLOSE, c, getLine(), getLinePos());
        case '[': return Token(BRAC_OPEN, c, getLine(), getLinePos());
        case ']': return Token(BRAC_CLOSE, c, getLine(), getLinePos());
        case '^': return Token(POWER, c, getLine(), getLinePos());
        case '=': return Token(EQ, c, getLine(), getLinePos());
        case ',': return Token(COMMA, c, getLine(), getLinePos());
        case '|': return Token(PIPE, c, getLine(), getLinePos());
        case ';': return Token(SEMI_COLON, c, getLine(), getLinePos());
        case '!': 
            {
            if (peekChar() == '/' && peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                auto comment = getToEOL();
                return Token(ECHO_COMMENT_LINE, comment, getLine(), getLinePos());
                }
            else if (match('/'))
                return Token(ECHO_END, "!/", getLine(), getLinePos()-1);
            else if (match('!'))
                return Token(ECHO_DOUBLE, "!", getLine(), getLinePos()-1);
            return Token(ECHO, c, getLine(), getLinePos());
            }
        case '.': 
            if (match('='))
                return Token(EQ_UNIT, getLine(), getLinePos()-1);
            else
                return Token(DOT, c, getLine(), getLinePos());
        case '+':
            if (match('='))
                {
                return Token(EQ_PLUS, getLine(), getLinePos()-1);
                }
            else if (match('+'))
                {
                return Token(INC, getLine(), getLinePos()-1);
                }
            return Token(PLUS, c, getLine(), getLinePos());
        case '-':
            if (match('='))
                {
                return Token(EQ_MIN, getLine(), getLinePos()-1);
                }
            else if (match('-'))
                {
                return Token(DEC, getLine(), getLinePos()-1);
                }
            return Token(MIN, c, getLine(), getLinePos());
        case '*':
            if (match('='))
                return Token(EQ_MULT, getLine(), getLinePos()-1);
            return Token(MULT, c, getLine(), getLinePos());
        case '#':
            if (match('/'))
                {
                return Token(MUTE_END, getLine(), getLinePos()-1);
                }
            else if (matchWord("define"))
                {
                auto l = getLine();
                auto p = getPos();
                skipWhiteSpace();
                return Token(DEFINE, getToEOL(), l, p);
                }
            return Token(MUTE_LINE, c, getLine(), getLinePos());
        case '/':
            {
            auto cc = peekChar();
            switch (cc)
                {
                case '=':
                    nextChar(); //consume
                    return Token(EQ_DIV, getLine(), getLinePos()-1);
                case '/':
                    {
                    nextChar(); //consume
                    auto comment = getToEOL();
                    return Token(COMMENT_LINE, comment, getLine(), getLinePos());
                    }
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return getNextToken();
                case '#':
                    nextChar(); //consume
                    return Token(MUTE_START, getLine(), getLinePos()-1);
                case '!':
                    nextChar(); //consume
                    return Token(ECHO_START, getLine(), getLinePos()-1);
                }
            return Token(DIV, c, getLine(), getLinePos());
            }
        case '\'':
            {
            auto start= peekedState.pos;
            auto curLine = peekedState.line;
            auto curLinePos = peekedState.linePos;
            skipToWithinLine('\'');
            return Token(QUOTED_STR, getText(start, std::max(start, peekedState.pos-1)), curLine, curLinePos);
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
                return Token(UNKNOWN, c, getLine(), getLinePos());
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

bool Tokenizer::peekWord(std::string str)
    {
    if(peekedState.pos + str.size() >= size)
        return false;
    if(str.compare(0, str.size(), _stream, peekedState.pos, str.size()) != 0)
        return false;
    if(peekedState.pos + str.size() == size)
        return true;//EOF, so ID-string matches.
    //looking for WORD, so next char should not be an ID char
    return !isIdChar(_stream[peekedState.pos+str.size()]);
    }

bool Tokenizer::matchWord(const std::string& str)
    {
    if (peekWord(str))
        {
        for(int i = 0; i < str.size(); i++)
            nextChar();//this function does some administration, so we can't just set this->pos.
        return true;
        }
    return false;
    }

