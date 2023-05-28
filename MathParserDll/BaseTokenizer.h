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
    public:
        struct State{
            Token token;
            TokenPos nextPos;
            bool newLineStarted = true;
            void clear() { token = Token::Null(); }
            bool isNull() { return token.isNull(); }
            };

    protected:
        const char* _stream;
        size_t size = -1;
        State state;
        bool newlineIsToken = false;
    public:
        const State& getState() const {return state; }
        unsigned int getPos() const { return state.nextPos.cursorPos;}
        const Token& getCurrentToken() const { return state.token; }
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
                    state.newLineStarted = true;
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
            if(state.nextPos.cursorPos >= size)
                return 0; //EOF
            return _stream[state.nextPos.cursorPos];
            }

        char peekSecondChar()
            {
            if((state.nextPos.cursorPos+1) >= size)
                return 0; //EOF
            return _stream[state.nextPos.cursorPos+1];
            }

        char nextChar()
            {
            if(state.nextPos.cursorPos >= size)
                return 0; //EOF
            if(_stream[state.nextPos.cursorPos] == '\n')
                {
                state.nextPos.line++;
                state.nextPos.linePos = 0;
                state.newLineStarted = true;
                }
            else
                state.nextPos.linePos++;
            return _stream[state.nextPos.cursorPos++];
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
            return state.token;
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

