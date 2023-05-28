#pragma once
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "Token.h"
#include "Number.h"
#include "ValueType.h"
#include "BaseTokenizer.h"

class Tokenizer : public BaseTokenizer<Token>
    {
    public:
        Tokenizer(const char* stream, char sourceIndex);
        Token next();
        void tokenizeComments(bool comments);
        void tokenizeNewlines(bool set);
        char sourceIndex;
        void setState(const State& newState);

    private:
        bool peekComments = false;
        bool peekWord(std::string str);
        Token parseId(char c);

        Number parseDecimal(char c);
        double parseBinary();
        double parseHex();
        int parseInteger();

        Token parseNumber(char c);

        void skipToEndOfComment();
        Token nextUnfiltered();
        bool matchWord(const std::string& str);
    };

