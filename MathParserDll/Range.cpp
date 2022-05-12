#include "pch.hpp"
#include "Range.h"
#include "Token.h"
#include <sstream>

Range& Range::operator+=(const Range& r)
    {
    if(r.start.line < 0) //means that r is empty
        return *this;
    if (r.start.line < start.line)
        {
        start = r.start;
        }
    else if(r.start.line == start.line)
        {
        start.linePos = std::min(start.linePos, r.start.linePos);
        start.cursorPos = std::min(start.cursorPos, r.start.cursorPos);
        }

    if (r.end.line > end.line)
        {
        end = r.end;
        }
    else if (r.end.line == end.line)
        {
        end.linePos = std::max(end.linePos, r.end.linePos);
        end.cursorPos = std::max(end.cursorPos, r.end.cursorPos);
        }
    return *this;
    }

Range::Range(const Token& t)
    {
    start = t.pos;
    end = t.pos;
    end.cursorPos += (unsigned)t.stringValue.size();
    end.linePos += (unsigned)t.stringValue.size();
    }

void Range::to_json(std::ostringstream& sstr) const
    {
    sstr << "{";
    sstr << "\"startLine\":" << start.line;
    sstr << ",\"startPos\":" << start.linePos;
    sstr << ",\"endLine\":" << end.line;
    sstr << ",\"endPos\":" << end.linePos;
    sstr << "}";
    }
