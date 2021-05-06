#include "pch.h"
#include "api.h"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"

int __cdecl getString(char* buffer, int strlen)
    {
    std::string result = "Tadaa!!!";

    result = result.substr(0, strlen);

    std::copy(result.begin(), result.end(), buffer);
    buffer[std::min(strlen - 1, (int)result.size())] = 0;
    return strlen;//TODO: pointless
    }

int __cdecl parse(const char* str)
    {
    Tokenizer tok = Tokenizer(str);
    int i = 0;
    while (true)
        {
        Token t = tok.next();
        if (t.type == TokenType::EOT)
            break;
        i++;
        }
    return i;
    }
