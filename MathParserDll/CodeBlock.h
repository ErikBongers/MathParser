#pragma once
#include <memory>
#include <vector>

class Scope;
class Statement;
struct CodeBlock
    {
    const char* _stream;
    std::unique_ptr<Scope> scope;
    std::vector<Statement*> statements;
    CodeBlock(std::unique_ptr<Scope>&& scope) : scope(std::move(scope)), _stream(nullptr) {}
    CodeBlock(std::unique_ptr<Scope>&& scope, std::vector<Statement*>&& statements, const char* stream) : scope(std::move(scope)), statements(std::move(statements)), _stream(stream) {}
    std::string getText(unsigned int start, unsigned end) { return std::string(&_stream[start], &_stream[end]); }
    std::string getText(const Range& range) { return std::string(&_stream[range.start.cursorPos], &_stream[range.end.cursorPos]); }

    };
