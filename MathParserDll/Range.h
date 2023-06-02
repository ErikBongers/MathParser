#pragma once
#include "TokenPos.h"

struct Range
    {
    TokenPos start;
    TokenPos end;
    Range& operator+=(const Range& r);
    Range() : start({ 0, 0, 0, -1}) {}
    Range(TokenPos start, TokenPos end) : start(start), end(end) {}
    void to_json(std::ostringstream& sstr) const;
    };

