#pragma once
#include "SourcePos.h"

struct Range
    {
    SourcePos start;
    SourcePos end;
    Range& operator+=(const Range& r);
    Range() : start({ 0, 0, 0, -1}), end({ 0, 0, 0, -1}) {}
    Range(SourcePos start, SourcePos end);
    bool isEmpty() const { return start.cursorPos == end.cursorPos; }
    bool isNone() const { return start.sourceIndex == -1; }
    void to_json(std::ostringstream& sstr) const;
    };

