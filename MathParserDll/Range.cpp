#include "pch.h"
#include "Range.h"
#include "Token.h"
#include <sstream>

Range& Range::operator+=(const Range& r)
    {
    if (r.startLine < startLine)
        {
        startLine = r.startLine;
        startPos = r.startPos;
        }
    else if(r.startLine == startLine)
        startPos = std::min(startPos, r.startPos);

    if (r.endLine > endLine)
        {
        endLine = r.endLine;
        endPos = r.endPos;
        }
    else if (r.endLine == endLine)
        endPos = std::max(endPos, r.endPos);
    return *this;
    }

Range::Range(const Token& t)
    {
    startLine = endLine = t.line; //TODO: assuming token on ONE line.
    startPos = t.pos;
    endPos = t.pos + (unsigned)t.stringValue.size();
    }

const std::string Range::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";
    sstr << "\"startLine\":" << startLine;
    sstr << ",\"startPos\":" << startPos;
    sstr << ",\"endLine\":" << endLine;
    sstr << ",\"endPos\":" << endPos;
    sstr << "}";
    return sstr.str();
    }
