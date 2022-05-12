#pragma once
#include "TokenPos.h"
/*
Token is a class that must implement following functions:
    static Token Null();
    bool isNull() const;
*/

template <class Token>
class BaseTokenizer
    {
    protected:
        const char* _stream;
        size_t size = -1;
        struct State{
            Token token;
            TokenPos nextPos;
            bool newLineStarted = true;
            void clear() { token = Token::Null(); }
            bool isNull() { return token.isNull(); }
            };
        State peekedState;
        State currentState;
        bool newlineIsToken = false;
        unsigned int getLine() { return peekedState.nextPos.line;}
        unsigned int getLinePos() { return peekedState.nextPos.linePos-1;}
    public:
        unsigned int getPos() { return peekedState.nextPos.cursorPos;}
        const Token& getCurrentToken() const { return currentState.token; }
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
        BaseTokenizer(const char* stream) 
            : _stream(stream) 
            { 
            size = strlen(stream); 
            }

        void skipWhiteSpace()
            {
            char c;
            while ((c = peekChar()))
                {
                if(c == '\n')
                    {
                    peekedState.newLineStarted = true;
                    if(newlineIsToken)
                        break;
                    }
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                    break;
                nextChar(); //consume
                }
            }

        void skipWhiteSpaceNoNL()
            {
            char c;
            while ((c = peekChar()))
                {
                if (c != ' ' && c != '\t')
                    break;
                nextChar(); //consume
                }
            }

        char peekChar()
            {
            if(peekedState.nextPos.cursorPos >= size)
                return 0; //EOF
            return _stream[peekedState.nextPos.cursorPos];
            }

        char peekSecondChar()
            {
            if((peekedState.nextPos.cursorPos+1) >= size)
                return 0; //EOF
            return _stream[peekedState.nextPos.cursorPos+1];
            }

        char nextChar()
            {
            if(peekedState.nextPos.cursorPos >= size)
                return 0; //EOF
            if(_stream[peekedState.nextPos.cursorPos] == '\n')
                {
                peekedState.nextPos.line++;
                peekedState.nextPos.linePos = 0;
                peekedState.newLineStarted = true;
                }
            else
                peekedState.nextPos.linePos++;
            return _stream[peekedState.nextPos.cursorPos++];
            }

        bool match(char c)
            {
            if (peekChar() == c)
                {
                nextChar();
                return true;
                }
            return false;
            }

        Token peek()
            {
            return peekedState.token;
            }

        void skipToEOL()
            {
            char c;
            while ((c = nextChar()))
                {
                if(c == '\n')
                    break;
                }
            }

        void skipToWithinLine(char c)
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

        std::string getToEOL()
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


    };

