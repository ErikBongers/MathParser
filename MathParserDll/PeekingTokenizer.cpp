#include "pch.hpp"
#include "PeekingTokenizer.h"

PeekingTokenizer::PeekingTokenizer(const char* stream, char sourceIndex)
    : tok(stream, sourceIndex)
    {
    next();
    }

Token PeekingTokenizer::next()
    {
    currentState = tok.getState(); //tok contains the peeked token that we are consuming, making current and returning
    tok.next(); //setting the next peek ready.
    return currentState.token;
    }

Token PeekingTokenizer::peek()
    {
    return tok.getCurrentToken();
    }

Token PeekingTokenizer::peekSecond()
    {
    auto tmpState = tok.getState();
    tok.next();
    auto t = tok.getCurrentToken();
    tok.setState(tmpState);
    return t;

    }

void PeekingTokenizer::tokenizeComments(bool comments)
    {
    tok.setState(currentState);
    tok.tokenizeComments(comments);
    tok.next();
    }

void PeekingTokenizer::tokenizeNewlines(bool set)
    {
    tok.setState(currentState);
    tok.tokenizeNewlines(set);
    tok.next();
    }

std::string PeekingTokenizer::getText(const Range& range)
    {
    return getText(range.start.cursorPos, range.end.cursorPos);
    }

std::string PeekingTokenizer::getText(unsigned int start, unsigned end)
    {
    return tok.getText(start, end);
    }

const Token& PeekingTokenizer::getCurrentToken() const
    {
    return currentState.token;
    }

TokenPos PeekingTokenizer::getPos() const
    {
    return currentState.nextPos;
    }

unsigned int PeekingTokenizer::getSourceIndex() const
    {
    return tok.sourceIndex;
    }
