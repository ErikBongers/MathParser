#pragma once
#include <memory>
#include <vector>

class Scope;
class Statement;
struct CodeBlock
    {
    std::unique_ptr<Scope> scope;
    std::vector<Statement*> statements;
    CodeBlock(std::unique_ptr<Scope>&& scope) : scope(std::move(scope)) {}
    CodeBlock(std::unique_ptr<Scope>&& scope, std::vector<Statement*>&& statements) : scope(std::move(scope)), statements(std::move(statements)) {}
    };
