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
const auto ERROR_SOURCE = "ERROR: parse failed to find the specified source files.";

int C_DECL setSource(const char* scriptId, const char* text)
    {
    auto found = std::find_if(sources.begin(), sources.end(), [&scriptId] (Source& source) {
        return source.name == scriptId;
                              });
    if (found != std::end(sources))
        {
        found->text = text;
        return found->index;
        }
    else
        {
        sources.push_back(Source{(char)sources.size(), scriptId, text});
        return (int)sources.size()-1;
        }
    }

char getSourceIndex(const char* scriptId)
    {
    auto found = std::find_if(sources.begin(), sources.end(), 
        [&scriptId] (Source& source) {
            return source.name == scriptId;
        });
    if (found != std::end(sources))
        return (*found).index;
    return -1;
    }


int C_DECL parse(const char* startScriptId, const char* mainScriptId)
    {
    auto mainIndex = getSourceIndex(mainScriptId);
    if (mainIndex == -1)
        {
        result = ERROR_SOURCE;
        return -1;
        }
    auto startIndex = getSourceIndex(startScriptId);
    if(startIndex == -1 && strlen(startScriptId) != 0)
        {
        result = ERROR_SOURCE;
        return -1;
        }
    auto firstIndex = startIndex != -1 ? startIndex : mainIndex;
    auto secondIndex = startIndex != -1 ? mainIndex : -1;

    Globals globals(sources);
    auto scope = std::make_unique<Scope>(globals);
    NodeFactory nodeFactory;
    CodeBlock codeBlock(std::move(scope));

    PeekingTokenizer tok(sources[firstIndex]);
    Parser parser(codeBlock, nodeFactory, tok);
    parser.parse();
    
    if(secondIndex != -1)
        {
        PeekingTokenizer tok2(globals.sources[secondIndex]);
        Parser parser(codeBlock, nodeFactory, tok2);
        parser.parse();
        }
    Resolver resolver(codeBlock);
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

