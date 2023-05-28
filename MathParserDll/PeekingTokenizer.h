#pragma once
#include "Tokenizer.h"

class PeekingTokenizer
    {
    private:
        Tokenizer tok;
        BaseTokenizer<Token>::State currentState;

    public:
        PeekingTokenizer(const char* stream, char sourceIndex);
        Token next();
        Token peek();
        Token peekSecond();
        void tokenizeComments(bool comments);
        void tokenizeNewlines(bool set);
        std::string getText(unsigned int start, unsigned end);
        const Token& getCurrentToken() const;
        unsigned int getPos() const; //TODO: return a Pos object instead of int. Also see getSourceIndex()
        unsigned int getSourceIndex() const; //TODO: try to get rid of this function by including SourceIndex into Pos and Range
    };