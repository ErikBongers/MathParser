#pragma once

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
            unsigned int pos = 0; //pos at which to read next token.
            unsigned int line = 0;
            unsigned int linePos = 0;
            bool newLineStarted = true;
            void clear() { token = Token::Null(); }
            bool isNull() { return token.isNull(); }
            };
        State peekedState;
    public:
        unsigned int getLine() { return peekedState.line;}
        unsigned int getLinePos() { return peekedState.linePos-1;} //linePos always contains the NEXT pos.
        unsigned int getPos() { return peekedState.pos;} //pos always contains the NEXT pos.
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
            if(peekedState.pos >= size)
                return 0; //EOF
            return _stream[peekedState.pos];
            }

        char peekSecondChar()
            {
            if((peekedState.pos+1) >= size)
                return 0; //EOF
            return _stream[peekedState.pos+1];
            }

        char nextChar()
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

