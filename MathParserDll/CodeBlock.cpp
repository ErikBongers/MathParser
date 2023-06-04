#include "pch.hpp"
#include "CodeBlock.h"
#include "Scope.h"

CodeBlock::CodeBlock(std::unique_ptr<Scope>&& scope) : scope(std::move(scope)) 
    {
    }

CodeBlock::CodeBlock(std::unique_ptr<Scope>&& scope, std::vector<Statement*>&& statements) 
    : scope(std::move(scope)), statements(std::move(statements)) 
    {
    }
