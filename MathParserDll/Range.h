#pragma once
#include "TokenPos.h"

class Token;
struct Range
    {
    TokenPos start;

    TokenPos end;
    Range& operator+=(const Range& r);
    Range() : start({ -1, -1, -1}) {}
    Range(const Token& t);
    Range(TokenPos start, TokenPos end) : start(start), end(end) {}
    void to_json(std::ostringstream& sstr) const;
    };

