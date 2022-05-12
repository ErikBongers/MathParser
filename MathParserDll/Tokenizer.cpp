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
    currentState = peekedState;
    Token t = currentState.token;
    getNextState();
    return t;
    }

void Tokenizer::tokenizeComments(bool comments) 
    { 
    peekComments = comments;
    peekedState = currentState; //rollback last peek, and peek again.
    getNextState();
    }

void Tokenizer::tokenizeNewlines(bool set) 
    { 
    newlineIsToken = set;
    peekedState = currentState; //rollback last peek, and peek again.
    getNextState();
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
    if (peekedState.nextPos.cursorPos >= size)
        return Token(TokenType::EOT, peekedState.nextPos);

    skipWhiteSpace();

    if(matchWord("function"))
        return Token(TokenType::FUNCTION, "function", peekedState.nextPos);

    char c;
    c = nextChar();
    if (!c)
        return Token(TokenType::EOT, peekedState.nextPos);

    switch (c)
        {
        using enum TokenType;
        case '\n': return Token(NEWLINE, c, peekedState.nextPos);
        case '{': return Token(CURL_OPEN, c, peekedState.nextPos);
        case '}': return Token(CURL_CLOSE, c, peekedState.nextPos);
        case '(': return Token(PAR_OPEN, c, peekedState.nextPos);
        case ')': return Token(PAR_CLOSE, c, peekedState.nextPos);
        case '[': return Token(BRAC_OPEN, c, peekedState.nextPos);
        case ']': return Token(BRAC_CLOSE, c, peekedState.nextPos);
        case '^': return Token(POWER, c, peekedState.nextPos);
        case '=': return Token(EQ, c, peekedState.nextPos);
        case ',': return Token(COMMA, c, peekedState.nextPos);
        case '|': return Token(PIPE, c, peekedState.nextPos);
        case ';': return Token(SEMI_COLON, c, peekedState.nextPos);
        case '!': 
            {
            if (peekChar() == '/' && peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                auto comment = getToEOL();
                return Token(ECHO_COMMENT_LINE, comment, peekedState.nextPos);
                }
            else if (match('/'))
                return Token(ECHO_END, "!/", peekedState.nextPos.offset(-1));
            else if (match('!'))
                return Token(ECHO_DOUBLE, "!", peekedState.nextPos.offset(-1));
            return Token(ECHO, c, peekedState.nextPos);
            }
        case '.': 
            if (match('='))
                return Token(EQ_UNIT, peekedState.nextPos.offset(-1));
            else
                return Token(DOT, c, peekedState.nextPos);
        case '+':
            if (match('='))
                {
                return Token(EQ_PLUS, peekedState.nextPos.offset(-1));
                }
            else if (match('+'))
                {
                return Token(INC, peekedState.nextPos.offset(-1));
                }
            return Token(PLUS, c, peekedState.nextPos);
        case '-':
            if (match('='))
                {
                return Token(EQ_MIN, peekedState.nextPos.offset(-1));
                }
            else if (match('-'))
                {
                return Token(DEC, peekedState.nextPos.offset(-1));
                }
            return Token(MIN, c, peekedState.nextPos);
        case '*':
            if (match('='))
                return Token(EQ_MULT, peekedState.nextPos.offset(-1));
            return Token(MULT, c, peekedState.nextPos);
        case '#':
            if (match('/'))
                {
                return Token(MUTE_END, peekedState.nextPos.offset(-1));
                }
            else if (matchWord("define"))
                {
                return Token(DEFINE, "#define", peekedState.nextPos);
                }
            return Token(MUTE_LINE, c, peekedState.nextPos);
        case '/':
            {
            auto cc = peekChar();
            switch (cc)
                {
                case '=':
                    nextChar(); //consume
                    return Token(EQ_DIV, peekedState.nextPos.offset(-1));
                case '/':
                    {
                    nextChar(); //consume
                    auto comment = getToEOL();
                    return Token(COMMENT_LINE, comment, peekedState.nextPos);
                    }
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return getNextToken();
                case '#':
                    nextChar(); //consume
                    return Token(MUTE_START, peekedState.nextPos.offset(-1));
                case '!':
                    nextChar(); //consume
                    return Token(ECHO_START, peekedState.nextPos.offset(-1));
                }
            return Token(DIV, c, peekedState.nextPos);
            }
        case '\'':
            {
            auto start= peekedState.nextPos;
            skipToWithinLine('\'');
            return Token(QUOTED_STR, getText(start.cursorPos, std::max(start.cursorPos, peekedState.nextPos.cursorPos-1)), start);
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
                return Token(UNKNOWN, c, peekedState.nextPos);
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
    auto wordPos = peekedState.nextPos;
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
    return Token(TokenType::ID, word, wordPos);
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
    auto curPos = peekedState.nextPos;
    if(peekChar() == 'b' || peekChar() == 'B')
        return Token(TokenType::NUMBER, Number(parseBinary(), 0, NumFormat::BIN), curPos);
    else if(peekChar() == 'x' || peekChar() == 'X')
        return Token(TokenType::NUMBER, Number(parseHex(), 0, NumFormat::HEX), curPos);
    else
        return Token(TokenType::NUMBER, parseDecimal(c), curPos);
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
    if(peekedState.nextPos.cursorPos + str.size() >= size)
        return false;
    if(str.compare(0, str.size(), _stream, peekedState.nextPos.cursorPos, str.size()) != 0)
        return false;
    if(peekedState.nextPos.cursorPos + str.size() == size)
        return true;//EOF, so ID-string matches.
    //looking for WORD, so next char should not be an ID char
    return !isIdChar(_stream[peekedState.nextPos.cursorPos+str.size()]);
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

