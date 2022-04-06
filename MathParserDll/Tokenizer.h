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
        Tokenizer(const char* stream);
        Token peek();
        Token peekSecond();
        Token next();
        unsigned int getLine() { return peekedState.line;}
        unsigned int getLinePos() { return peekedState.linePos-1;} //linePos always contains the NEXT pos.
        unsigned int getPos() { return peekedState.pos;} //pos always contains the NEXT pos.
        std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
        void skipWhiteSpace();
        void skipWhiteSpaceNoNL();

        bool peekComments = false;
    private:
        void getNextState();
        char nextChar();
        inline char peekChar();
        inline char peekSecondChar();
        bool peekWord(std::string str);
        Token parseId(char c);

        Number parseDecimal(char c);
        double parseBinary();
        double parseHex();
        int parseInteger();

        Token parseNumber(char c);

        void skipToEOL();
        std::string getToEOL();
        void skipToEndOfComment();
        Token getNextToken();
        void skipToOnLine(char c);
        bool match(char c);
        bool matchWord(const std::string& str);
    };

