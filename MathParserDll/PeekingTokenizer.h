#pragma once
#include "Tokenizer.h"

class PeekingTokenizer
    {
    private:
        Tokenizer tok;
        BaseTokenizer<Token>::State currentState;

    public:
        PeekingTokenizer(const Source& source);
        Token next();
        Token peek();
        Token peekSecond();
        void tokenizeComments(bool comments);
        void tokenizeNewlines(bool set);
        std::string getText(const Range&range);
        std::string getText(unsigned int start, unsigned end);
        const Token& getCurrentToken() const;
    };