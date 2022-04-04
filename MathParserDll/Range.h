#pragma once

class Token;
struct Range
    {
    unsigned startLine;
    unsigned startPos;

    unsigned endLine;
    unsigned endPos;
    Range& operator+=(const Range& r);
    Range() {};
    Range(const Token& t);
    Range(unsigned i1, unsigned i2, unsigned i3, unsigned i4) : startLine(i1), startPos(i2), endLine(i3), endPos(i4) {}
    const std::string to_json();
    };

