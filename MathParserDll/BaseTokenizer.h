#pragma once
#include "TokenPos.h"
#include "Source.h"

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
        const Source& source;
        const char* _stream;
        size_t size = -1;
        State state;
        bool newlineIsToken = false;
    public:
        const State& getState() const {return state; }
        unsigned int getPos() const { return state.nextPos.cursorPos;}
        const Token& getCurrentToken() const { return state.token; }
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
        BaseTokenizer(const Source& source)
            : source(source), _stream(source.text.c_str())
            { 
            size = strlen(_stream); 
            state.nextPos.sourceIndex = source.index;
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

        TokenPos getToWithinLine(char c)
            {
            char cc;
            while ((cc = peekChar()))
                {
                if(cc == '\n')
                    break;
                if(c == cc)
                    break;
                nextChar();
                }
            TokenPos pos = state.nextPos;
            match(c);
            return pos;
            }

        TokenPos getToEOL()
            {
            char c;
            while ((c = peekChar())) 
                {
                if(c == '\r' || c == '\n')
                    break; //don't eat NL yet. NL could be a token!
                nextChar();
                }
            //currently at \r or \n
            TokenPos pos = state.nextPos;
            match('\r'); //ignore \r. It's not part of the string and never a token.
            return pos;
            }


    };

