#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Token.h"
#include "Number.h"
#include "ValueType.h"

class Tokenizer
    {
    private:
        const char* _stream;
        size_t size = -1;
        struct State{
            Token currentToken;
            unsigned int pos = 0; //pos at which to read next token.
            unsigned int line = 0;
            unsigned int linePos = 0;
            bool newLineStarted = true;
            void clear() { currentToken = Token::Null(); }
            bool isNull() { return currentToken.isNull(); }
            };
        State state;
        State peekedState;
    public:
        Tokenizer(const char* stream) : _stream(stream) { size = strlen(stream); }
        Token peek(bool includeComment = false);
        Token peekSecond(bool includeComment = false);
        Token next(bool includeComment = false);
        unsigned int getLine() { return state.line;}
        unsigned int getLinePos() { return state.linePos-1;} //linePos always contains the NEXT pos.
        unsigned int getPos() { return state.pos;} //pos always contains the NEXT pos.
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
        void skipWhiteSpace();
        void skipWhiteSpaceNoNL();

    private:
        char nextChar();
        inline char peekChar();
        inline char peekSecondChar();
        bool peekString(std::string str);
        bool getString(std::string str);
        Token parseId(char c);

        Number parseDecimal(char c);
        double parseBinary();
        double parseHex();
        int parseInteger();

        Token parseNumber(char c);

        void skipToEOL();
        std::string getToEOL();
        void skipToEndOfComment();
        Token _nextToken();
        void skipToOnLine(char c);
    };

