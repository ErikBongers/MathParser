#pragma once
#include "TokenPos.h"

class Token;
struct Range
    {
    TokenPos start;
    TokenPos end;
    char sourceIndex;
    Range& operator+=(const Range& r);
    Range() : sourceIndex(-1), start({ -1, -1, -1}) {}
    Range(const Token& t);
    Range(TokenPos start, TokenPos end, char sourceIndex) : start(start), end(end), sourceIndex(sourceIndex) {}
    void to_json(std::ostringstream& sstr) const;
    };

