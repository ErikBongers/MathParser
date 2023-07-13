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
std::vector<Source> sources;

void C_DECL setSource(const char* scriptId, const char* text)
    {
    auto found = std::find_if(sources.begin(), sources.end(), [&scriptId] (Source& source) {
        return source.name == scriptId;
                              });
    if (found != std::end(sources))
        (*found).text = text;
    else
        sources.push_back(Source((char)sources.size(), scriptId, text));
    }

char getSourceIndex(const char* scriptId)
    {
    auto found = std::find_if(sources.begin(), sources.end(), [&scriptId] (Source& source) {
        return source.name == scriptId;
                              });
    if (found != std::end(sources))
        return (*found).index;
    return -1; //TODO: sentinel value
    }

int C_DECL parse(const char* scriptId)
    {
    Globals globals(sources);
    auto sourceIndex = getSourceIndex(scriptId);//TODO: this could fail!!
    auto scope = std::make_unique<Scope>(globals);
    NodeFactory nodeFactory;
    CodeBlock codeBlock(std::move(scope));
    PeekingTokenizer tok(globals.sources[sourceIndex].text.c_str(), 0);
    Parser parser(codeBlock, nodeFactory, tok);
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

