#include "pch.h"
#include "json.h"

const std::string escape_json(const std::string& s)
    {
    std::string newStr;
    for (auto& c : s)
        {
        if (c == '\"' || c == '\\' || c == '/')
            {
            newStr += '\\';
            newStr += c;
            }
        else if (c == '\n')
            {
            newStr += "\\n";
            }
        else if (c == '\r')
            {
            newStr += "\\r";
            }
        else
            newStr += c;
        }
    return newStr;
    }

const std::string make_one_line(const std::string& s)
    {
    std::string newStr = s;
    newStr.erase(remove(newStr.begin(), newStr.end(), '\r'), newStr.end());
    newStr.erase(remove(newStr.begin(), newStr.end(), '\n'), newStr.end());
    return newStr;
    }