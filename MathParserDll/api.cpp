#include "pch.h"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"
#include "Resolver.h"
#include "api.h"
#include "version.h"

std::string version = "1.0." + std::to_string(VERSION_BUILD);

int C_DECL parse(const char* str)
    {
    UnitDef::init(); //todo: unsafe: parser and resolver require this. use dependency injection or singleton.
    Parser parser(str);
    Resolver resolver(parser);
    resolver.resolve();
    return getResultLen();
    }

int C_DECL getResultLen() { return Resolver::getResultLen() + 1; }

void trimStringAndCopy(std::string& str, char* buffer, int buflen)
    {
    if(buflen <= 0)
        return;
    if(str.size() >= buflen)
        str.resize(buflen-1);
    std::copy(str.begin(), str.end(), buffer);
    buffer[(int)str.size()] = 0;
    }

void C_DECL getResult(char* buffer, int buflen)
    {
    auto result = Resolver::getResult();
    trimStringAndCopy(result, buffer, buflen);
    }

void C_DECL getVersion(char* buffer, int buflen)
    {
    auto copy = version;
    trimStringAndCopy(copy, buffer, buflen);
    }

