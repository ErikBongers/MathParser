#include "pch.hpp"
#include "Tokenizer.h"
#include <algorithm>

Tokenizer::Tokenizer(const char* stream, char sourceIndex) 
    : BaseTokenizer(stream), sourceIndex(sourceIndex)
    { 
    }

void Tokenizer::setState(const State& newState)
    {
    this->state = newState;
    }

void Tokenizer::tokenizeComments(bool comments) 
    { 
    peekComments = comments;
    }

void Tokenizer::tokenizeNewlines(bool set) 
    { 
    newlineIsToken = set;
    }

Token Tokenizer::next()
    {
    state.token = nextUnfiltered();
    if(peekComments == false)
        { 
        //skip comments
        while(state.token.type == TokenType::ECHO_COMMENT_LINE || state.token.type == TokenType::COMMENT_LINE)
            {
            state.token = nextUnfiltered();
            }
        }
    state.token.isFirstOnLine = state.newLineStarted;
    state.newLineStarted = false;
    return state.token;
    }

Token Tokenizer::nextUnfiltered()
    {
    if (state.nextPos.cursorPos >= size)
        return Token(TokenType::EOT, state.nextPos-1, sourceIndex);

    skipWhiteSpace();

    if(matchWord("function"))
        return Token(TokenType::FUNCTION, "function", state.nextPos-1, sourceIndex);

    char c;
    c = nextChar();
    if (!c)
        return Token(TokenType::EOT, state.nextPos-1, sourceIndex);

    switch (c)
        {
        using enum TokenType;
        case '\n': return Token(NEWLINE, c, state.nextPos-1, sourceIndex);
        case '{': return Token(CURL_OPEN, c, state.nextPos-1, sourceIndex);
        case '}': return Token(CURL_CLOSE, c, state.nextPos-1, sourceIndex);
        case '(': return Token(PAR_OPEN, c, state.nextPos-1, sourceIndex);
        case ')': return Token(PAR_CLOSE, c, state.nextPos-1, sourceIndex);
        case '[': return Token(BRAC_OPEN, c, state.nextPos-1, sourceIndex);
        case ']': return Token(BRAC_CLOSE, c, state.nextPos-1, sourceIndex);
        case '^': return Token(POWER, c, state.nextPos-1, sourceIndex);
        case '=': return Token(EQ, c, state.nextPos-1, sourceIndex);
        case ',': return Token(COMMA, c, state.nextPos-1, sourceIndex);
        case '|': return Token(PIPE, c, state.nextPos-1, sourceIndex);
        case ';': return Token(SEMI_COLON, c, state.nextPos-1, sourceIndex);
        case '%': 
            if(match('%'))
                return Token(MODULO, "%%", state.nextPos - 2, sourceIndex);
            return Token(PERCENT, c, state.nextPos-1, sourceIndex);
        case '!': 
            {
            if (peekChar() == '/' && peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                if (peekChar() == '/')
                    {
                    nextChar(); //consume
                    return Token(ECHO_START, "!///", state.nextPos-4, sourceIndex);
                    }
                else
                    {
                    auto comment = getToEOL();
                    return Token(ECHO_COMMENT_LINE, comment, state.nextPos-2, sourceIndex);
                    }
                }
            else if (match('!'))
                return Token(ECHO_DOUBLE, "!", state.nextPos-2, sourceIndex);
            return Token(EXCLAM, c, state.nextPos-1, sourceIndex);
            }
        case '.': 
            if (match('='))
                return Token(EQ_UNIT, state.nextPos-2, sourceIndex);
            if (peekChar() >= '0' && peekChar() <= '9')
                {
                return parseNumber('.');
                }
            else
                return Token(DOT, c, state.nextPos-1, sourceIndex);
        case '+':
            if (match('='))
                {
                return Token(EQ_PLUS, state.nextPos-2, sourceIndex);
                }
            else if (match('+'))
                {
                return Token(INC, state.nextPos-2, sourceIndex);
                }
            return Token(PLUS, c, state.nextPos-1, sourceIndex);
        case '-':
            if (match('='))
                {
                return Token(EQ_MIN, state.nextPos-2, sourceIndex);
                }
            else if (match('-'))
                {
                return Token(DEC, state.nextPos-2, sourceIndex);
                }
            return Token(MIN, c, state.nextPos-1, sourceIndex);
        case '*':
            if (match('='))
                return Token(EQ_MULT, state.nextPos-2, sourceIndex);
            return Token(MULT, c, state.nextPos-1, sourceIndex);
        case '#':
            if (match('/'))
                {
                return Token(MUTE_END, state.nextPos-2, sourceIndex);
                }
            else if (matchWord("define"))
                {
                return Token(DEFINE, "#define", state.nextPos-1, sourceIndex);
                }
            else if (matchWord("undef"))
                {
                return Token(UNDEF, "#undef", state.nextPos-1, sourceIndex);
                }
            return Token(MUTE_LINE, c, state.nextPos-1, sourceIndex);
        case '/':
            {
            auto cc = peekChar();
            switch (cc)
                {
                case '=':
                    nextChar(); //consume
                    return Token(EQ_DIV, state.nextPos-2, sourceIndex);
                case '/':
                    {
                    nextChar(); //consume
                    if (peekChar() == '/' && peekSecondChar() == '!')
                        {
                        nextChar();
                        nextChar();
                        return Token(ECHO_END, "///!", state.nextPos-4, sourceIndex);
                        }
                    auto comment = getToEOL();
                    return Token(COMMENT_LINE, comment, state.nextPos-1, sourceIndex);
                    }
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return nextUnfiltered();
                case '#':
                    nextChar(); //consume
                    return Token(MUTE_START, state.nextPos-2, sourceIndex);
                case '!':
                    nextChar(); //consume
                    return Token(ECHO_START, state.nextPos-2, sourceIndex);
                }
            return Token(DIV, c, state.nextPos-1, sourceIndex);
            }
        case '\'':
            {
            auto start= state.nextPos;
            skipToWithinLine('\'');
            return Token(QUOTED_STR, getText(start.cursorPos, std::max(start.cursorPos, state.nextPos.cursorPos-1)), start, sourceIndex);
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
                return Token(UNKNOWN, c, state.nextPos-1, sourceIndex);
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
    auto wordPos = state.nextPos;
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
    return Token(TokenType::ID, word, wordPos-1, sourceIndex);
    }

Number Tokenizer::parseDecimal(char c)
    {
    TokenPos startPos = state.nextPos-1;
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
    return Number(d, e, Range(startPos, state.nextPos, sourceIndex));
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
    auto curPos = state.nextPos;
    if(c == '0')
        {
        if(peekChar() == 'b' || peekChar() == 'B')
            {
            auto binary = parseBinary();
            return Token(TokenType::NUMBER, Number(binary, 0, Range(curPos, state.nextPos-1, sourceIndex), NumFormat::BIN), curPos, sourceIndex);
            }
        else if(peekChar() == 'x' || peekChar() == 'X')
            {
            auto hex = parseHex();
            return Token(TokenType::NUMBER, Number(hex, 0, Range(curPos, state.nextPos-1, sourceIndex), NumFormat::HEX), curPos, sourceIndex);
            }
        }
    return Token(TokenType::NUMBER, parseDecimal(c), curPos, sourceIndex);
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
    if(state.nextPos.cursorPos + str.size() >= size)
        return false;
    if(str.compare(0, str.size(), _stream, state.nextPos.cursorPos, str.size()) != 0)
        return false;
    if(state.nextPos.cursorPos + str.size() == size)
        return true;//EOF, so ID-string matches.
    //looking for WORD, so next char should not be an ID char
    return !isIdChar(_stream[state.nextPos.cursorPos+str.size()]);
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

