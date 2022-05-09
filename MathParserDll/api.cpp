#include "pch.h"
#include <string>
#include <algorithm>
#include <cstring>
#include "Tokenizer.h"
#include "Resolver.h"
#include "OperatorDef.h"
#include "api.h"
#include "version.h"

std::string version = "1.0." + std::to_string(VERSION_BUILD);

int C_DECL parse(const char* str)
    {
    UnitDefs unitDefs;
    FunctionDefs functionDefs(unitDefs);
    OperatorDefs operatorDefs(unitDefs);
    Parser parser(str, functionDefs);
    parser.parse();
    std::map<std::string, Value> nada;
    Resolver resolver(parser.statements, functionDefs, unitDefs, operatorDefs, nada);
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

