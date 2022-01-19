#include "pch.h"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"
#include "Resolver.h"
#include "api.h"

std::string version = "1.1314";

int C_DECL parse(const char* str)
    {
    Parser parser(str);
    Resolver resolver(parser);
    resolver.resolve();
    return resolver.getResultLen();
    }

int C_DECL getResultLen() { return Resolver::getResultLen() + 1; }

void C_DECL getResult(char* buffer, int strlen)
    {
    auto result = Resolver::getResult();
    result = result.substr(0, strlen);

    std::copy(result.begin(), result.end(), buffer);
    buffer[std::min(strlen - 1, (int)result.size())] = 0;
    }

void C_DECL getVersion(char* buffer, int strlen)
    {
    std::copy(version.begin(), version.end(), buffer); //TODO: replace with copy_n
    buffer[std::min(strlen - 1, (int)version.size())] = 0;
    }

