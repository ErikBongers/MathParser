#pragma once
#include "SourcePos.h"
#include "Source.h"
#include "SourceCharStream.h"
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
            StreamState streamState;
            void clear() { token = Token::Null(); }
            bool isNull() { return token.isNull(); }
            };

    protected:
        SourceCharStream stream;
        State state;
        bool newlineIsToken = false;
    public:
        const State& getState() const {return state; }
        void setState(const State& newState)
            {
            this->state = newState;
            stream.state = this->state.streamState;
            }

        unsigned int getPos() const { return state.nextPos.cursorPos;}
        const Token& getCurrentToken() const { return state.token; }
        std::string getText(unsigned int start, unsigned end) { return stream.getText(start, end); }
        BaseTokenizer(const Source& source)
            : stream(source)
            { 
            state.streamState.pos.sourceIndex = stream.source.index;
            }

        char nextChar()
            {
            char c = stream.nextChar();
            state.streamState = stream.state;
            return c;
            }

        bool match(char c)
            {
            bool b= stream.match(c);
            state.streamState = stream.state;
            return b;
            }

        void skipWhiteSpace()
            {
            char c;
            while ((c = stream.peekChar()))
                {
                if(c == '\n')
                    {
                    state.streamState.newLineStarted = true;
                    if(newlineIsToken)
                        break;
                    }
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r')
                    break;
                stream.nextChar();
                }
            //Do not set: state.streamState.newLineStarted = stream.state.newLineStarted;
            //that would override the set in the while loop!
            state.streamState.pos = stream.state.pos;
            }

        void skipWhiteSpaceNoNL()
            {
            char c;
            while ((c = stream.peekChar()))
                {
                if (c != ' ' && c != '\t')
                    break;
                nextChar(); //consume
                }
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

        SourcePos getToWithinLine(char c)
            {
            char cc;
            while ((cc = stream.peekChar()))
                {
                if(cc == '\n')
                    break;
                if(c == cc)
                    break;
                nextChar();
                }
            SourcePos pos = state.streamState.pos;
            match(c);
            return pos;
            }

        SourcePos getToEOL()
            {
            char c;
            while ((c = stream.peekChar())) 
                {
                if(c == '\r' || c == '\n')
                    break; //don't eat NL yet. NL could be a token!
                nextChar();
                }
            //currently at \r or \n
            SourcePos pos = state.streamState.pos;
            match('\r'); //ignore \r. It's not part of the string and never a token.
            return pos;
            }


    };

