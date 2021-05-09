#include "pch.h"
#include "api.h"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"
#include "Resolver.h"

int __cdecl parse(const char* str)
    {
    Parser parser(str);
    Resolver resolver(parser);
    resolver.resolve();
    return resolver.getResultLen();
    }

int __cdecl getResultLen() { return Resolver::getResultLen()+1; }

void __cdecl getResult(char* buffer, int strlen)
    {
    auto result = Resolver::getResult();
    result = result.substr(0, strlen);

    std::copy(result.begin(), result.end(), buffer);
    buffer[std::min(strlen - 1, (int)result.size())] = 0;
    }

