#include "pch.hpp"
#include "Tokenizer.h"
#include <algorithm>

Tokenizer::Tokenizer(const Source& source) 
    : BaseTokenizer(source)
    { 
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
    state.token.isFirstOnLine = state.streamState.newLineStarted;
    state.streamState.newLineStarted = false;
    return state.token;
    }

Token Tokenizer::nextUnfiltered()
    {
    if (state.streamState.pos.cursorPos >= stream.getSize())
        return Token(TokenType::EOT, 0, state.streamState.pos);

    skipWhiteSpace();

    if(matchWord("function"))
        return Token(TokenType::FUNCTION, 8, state.streamState.pos);

    char c;
    c = nextChar();
    if (!c)
        return Token(TokenType::EOT, 0, state.streamState.pos);

    switch (c)
        {
        using enum TokenType;
        case '\n': return Token(NEWLINE, 1, state.streamState.pos);
        case '{': return Token(CURL_OPEN, 1, state.streamState.pos);
        case '}': return Token(CURL_CLOSE, 1, state.streamState.pos);
        case '(': return Token(PAR_OPEN, 1, state.streamState.pos);
        case ')': return Token(PAR_CLOSE, 1, state.streamState.pos);
        case '[': return Token(BRAC_OPEN, 1, state.streamState.pos);
        case ']': return Token(BRAC_CLOSE, 1, state.streamState.pos);
        case '^': return Token(POWER, 1, state.streamState.pos);
        case '=': return Token(EQ, 1, state.streamState.pos);
        case ',': return Token(COMMA, 1, state.streamState.pos);
        case '|': return Token(PIPE, 1, state.streamState.pos);
        case ';': return Token(SEMI_COLON, 1, state.streamState.pos);
        case '%': 
            if(match('%'))
                return Token(MODULO, 2, state.streamState.pos);
            return Token(PERCENT, 1, state.streamState.pos);
        case '!': 
            {
            if (stream.peekChar() == '/' && stream.peekSecondChar() == '/')
                {
                nextChar(); //consume
                nextChar(); //consume
                if (stream.peekChar() == '/')
                    {
                    nextChar(); //consume
                    return Token(ECHO_START, 4, state.streamState.pos);
                    }
                else
                    {
                    auto startCommentPos = state.streamState.pos;
                    auto endCommentPos = getToEOL();
                    return Token(ECHO_COMMENT_LINE, startCommentPos, endCommentPos);
                    }
                }
            else if (match('!'))
                return Token(ECHO_DOUBLE, 2, state.streamState.pos);
            return Token(EXCLAM, 1, state.streamState.pos);
            }
        case '.': 
            if (stream.peekChar() == '.' && stream.peekSecondChar() == '.')
                {
                nextChar(); //consume
                nextChar(); //consume
                return Token(ELLIPSIS, 3, state.streamState.pos);
                }
            else if (match('='))
                return Token(EQ_UNIT, 2, state.streamState.pos);
            if (stream.peekChar() >= '0' && stream.peekChar() <= '9')
                {
                return parseNumber('.');
                }
            else
                return Token(DOT, 1, state.streamState.pos);
        case '+':
            if (match('='))
                {
                return Token(EQ_PLUS, 2, state.streamState.pos);
                }
            else if (match('+'))
                {
                return Token(INC, 2, state.streamState.pos);
                }
            return Token(PLUS, 1, state.streamState.pos);
        case '-':
            if (match('='))
                {
                return Token(EQ_MIN, 2, state.streamState.pos);
                }
            else if (match('-'))
                {
                return Token(DEC, 2, state.streamState.pos);
                }
            return Token(MIN, 1, state.streamState.pos);
        case '*':
            if (match('='))
                return Token(EQ_MULT, 2, state.streamState.pos);
            return Token(MULT, 1, state.streamState.pos);
        case '#':
            if (match('/'))
                {
                return Token(MUTE_END, 2, state.streamState.pos);
                }
            else if (matchWord("define"))
                {
                return Token(DEFINE, 7, state.streamState.pos);
                }
            else if (matchWord("undef"))
                {
                return Token(UNDEF, 6, state.streamState.pos);
                }
            return Token(MUTE_LINE, 1, state.streamState.pos);
        case '/':
            {
            auto cc = stream.peekChar();
            switch (cc)
                {
                case '=':
                    nextChar(); //consume
                    return Token(EQ_DIV, 2, state.streamState.pos);
                case '/':
                    {
                    nextChar(); //consume
                    if (stream.peekChar() == '/' && stream.peekSecondChar() == '!')
                        {
                        nextChar();
                        nextChar();
                        return Token(ECHO_END, 4, state.streamState.pos);
                        }
                    auto startCommentPos = state.streamState.pos;
                    auto endCommentPos = getToEOL();
                    return Token(COMMENT_LINE, startCommentPos, endCommentPos);
                    }
                case '*':
                    nextChar(); //consume
                    skipToEndOfComment();
                    return nextUnfiltered();
                case '#':
                    nextChar(); //consume
                    return Token(MUTE_START, 2, state.streamState.pos);
                case '!':
                    nextChar(); //consume
                    return Token(ECHO_START, 2, state.streamState.pos);
                }
            return Token(DIV, 1, state.streamState.pos-1);
            }
        case '\'':
            {
            auto start= state.streamState.pos;

            auto end = getToWithinLine('\'');;
            return Token(QUOTED_STR, start, end);
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
                return Token(UNKNOWN, 1, state.streamState.pos);
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
    auto wordPos = state.streamState.pos-1; //c is already consumed!

    while ((c = stream.peekChar()))
        {
        if (isIdChar(c))
            {
            nextChar(); //consume
            }
        else
            {
            break;
            }
        }
    return Token(TokenType::ID, wordPos, state.streamState.pos);
    }

Number Tokenizer::parseDecimal(char c)
    {
    SourcePos startPos = state.streamState.pos-1;
    //we already have the first digit
    double d = 0;
    int e = 0;
    double decimalDivider = 1;
    if (c == '.')
        decimalDivider = 10;
    else
        d = c - '0';

    while ((c = stream.peekChar()))
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
            char cc = stream.peekSecondChar();
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
    if (stream.peekChar() == 'e' || stream.peekChar() == 'E')
        {
        nextChar();//consume 'E'
        e = parseInteger();
        }
    return Number(d, e, Range(startPos, state.streamState.pos));
    }

int Tokenizer::parseInteger()
    {
    int i = 0;
    char c;
    int factor = 1;
    if (stream.peekChar() == '-')
        {
        nextChar();
        factor = -1;
        }
    else if (stream.peekChar() == '+')
        {
        nextChar();
        }

    while ((c = stream.peekChar()))
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

    while(stream.peekChar() == '0' || stream.peekChar() == '1' || stream.peekChar() == '_')
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

    while((stream.peekChar() >= '0' && stream.peekChar() <= '9') 
          || (stream.peekChar() >= 'a' && stream.peekChar() <= 'f') 
          || (stream.peekChar() >= 'A' && stream.peekChar() <= 'F') 
          || stream.peekChar() == '_')
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
    auto curPos = state.streamState.pos;
    if(c == '0')
        {
        if(stream.peekChar() == 'b' || stream.peekChar() == 'B')
            {
            auto binary = parseBinary();
            return Token(TokenType::NUMBER, Number(binary, 0, Range(curPos, state.streamState.pos-1), NumFormat::BIN), curPos, state.streamState.pos-1);
            }
        else if(stream.peekChar() == 'x' || stream.peekChar() == 'X')
            {
            auto hex = parseHex();
            return Token(TokenType::NUMBER, Number(hex, 0, Range(curPos, state.streamState.pos-1), NumFormat::HEX), curPos, state.streamState.pos-1);
            }
        }
    return Token(TokenType::NUMBER, parseDecimal(c), curPos, state.streamState.pos-1);
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
        if (stream.peekChar() == '/')
            {
            nextChar(); //consume
            return;
            }
        if(!stream.peekChar())
            return;
        }
    }


bool Tokenizer::peekWord(std::string str)
    {
    if(state.streamState.pos.cursorPos + str.size() >= stream.getSize())
        return false;
    if(str.compare(0, str.size(), stream.c_str(), state.streamState.pos.cursorPos, str.size()) != 0)
        return false;
    if(state.streamState.pos.cursorPos + str.size() == stream.getSize())
        return true;//EOF, so ID-string matches.
    //looking for WORD, so next char should not be an ID char
    return !isIdChar(stream.c_str()[state.streamState.pos.cursorPos+str.size()]);
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

