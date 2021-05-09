#include "pch.h"
#include "json.h"

const std::string escape_json(const std::string& s)
    {
    std::string newStr;
    for (auto& c : s)
        {
        if (c == '\"' || c == '\\' || c == '/')
            newStr += '\\';
        newStr += c;
        }
    return newStr;
    }
