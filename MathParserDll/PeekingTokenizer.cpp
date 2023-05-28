#include "pch.hpp"
#include "PeekingTokenizer.h"

Token PeekingTokenizer::next()
    {
    return tok.next();
    }

Token PeekingTokenizer::peek()
    {
    return tok.peek();
    }

Token PeekingTokenizer::peekSecond()
    {
    return tok.peekSecond();
    }

void PeekingTokenizer::tokenizeComments(bool comments)
    {
    tok.tokenizeComments(comments);
    }

void PeekingTokenizer::tokenizeNewlines(bool set)
    {
    tok.tokenizeNewlines(set);
    }

std::string PeekingTokenizer::getText(unsigned int start, unsigned end)
    {
    return tok.getText(start, end);
    }

const Token& PeekingTokenizer::getCurrentToken() const
    {
    return tok.getCurrentToken();
    }

void PeekingTokenizer::skipWhiteSpace()
    {
    tok.skipWhiteSpace();
    }

unsigned int PeekingTokenizer::getPos() const
    {
    return tok.getPos();
    }

unsigned int PeekingTokenizer::getSourceIndex() const
    {
    return tok.sourceIndex;
    }
