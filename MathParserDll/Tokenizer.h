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
        Tokenizer(const char* stream);
        Token peekSecond();
        Token next();
        void tokenizeComments(bool comments);

    private:
        bool peekComments = false;
        void getNextState();
        bool peekWord(std::string str);
        Token parseId(char c);

        Number parseDecimal(char c);
        double parseBinary();
        double parseHex();
        int parseInteger();

        Token parseNumber(char c);

        void skipToEndOfComment();
        Token getNextToken();
        bool matchWord(const std::string& str);
    };

