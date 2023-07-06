#include "pch.hpp"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"
#include "Resolver.h"
#include "Globals.h"
#include "api.h"
#include "version.h"
#include "Scope.h"

std::string version = "1.0." + std::to_string(VERSION_BUILD);
std::string result = "";

int C_DECL parse(const char* str)
    {
    Globals globals;
    auto scope = std::make_unique<Scope>(globals);
    scope->sources.push_back(str);
    NodeFactory nodeFactory;
    Parser parser(std::move(scope), 0, nodeFactory);
    parser.parse();
    std::map<std::string, Value> nada;
    Resolver resolver(parser.codeBlock);
    result = resolver.resolve();
    return getResultLen();
    }

int C_DECL getResultLen() { return ((int)result.size()) + 1; }

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
    trimStringAndCopy(result, buffer, buflen);
    }

void C_DECL getVersion(char* buffer, int buflen)
    {
    auto copy = version;
    trimStringAndCopy(copy, buffer, buflen);
    }

