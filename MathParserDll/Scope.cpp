#include "pch.hpp"
#include "Scope.h"
#include "Globals.h"
#include "Unit.h"
#include "Function.h"
#include "Parser.h"

Scope::Scope(Globals& globals) 
    : globals(globals), units(globals), functions(globals), constants(globals)
    {
    }

Scope::~Scope()
    {
    for(auto& f : localFunctions)
        delete f.second;
    }

std::unique_ptr<Scope> Scope::copyForScript()
    {
    auto newScope = std::make_unique<Scope>(globals);
    newScope->parentScope = this;
    return newScope; 
    }

std::unique_ptr<Scope> Scope::copyForBlock()
    {
    auto newScope = std::make_unique<Scope>(globals);
    newScope->parentScope = this;
    newScope->units.setDefs(units);
    newScope->sources = sources;
    //settings;
    newScope->dateFormat = dateFormat;
    newScope->strict = strict;
    //don't copy the variables!

    return newScope;
    }

bool Scope::functionExists(const std::string name) const
    {
    if(localFunctions.count(name) != 0)
        return true;
    else if(parentScope != nullptr && parentScope->functionExists(name))
        return true;
    else 
        return functions.exists(name);
    }

CustomFunction* Scope::getLocalFunction(const std::string& name)
    {
    if (localFunctions.count(name) == 0)
        return nullptr;
    return localFunctions.find(name)->second;
    }

FunctionDef* Scope::getFunction(const std::string& name)
    {
    FunctionDef* fd = getLocalFunction(name);
    if(fd != nullptr)
        return fd;
    if(parentScope != nullptr)
        fd = parentScope->getFunction(name);
    if(fd != nullptr)
        return fd;
    return functions.get(name);
    }

Value& Scope::getVariable(const std::string& id)
    {
    if(variables.count(id) != 0)
        return variables.find(id)->second;
    else 
        return constants.get(id);
    }

bool Scope::varExists(const std::string& id)
    {
    if(variables.count(id) != 0)
        return true;
    else
        return constants.exists(id);
    }

bool Scope::varDefExists(const std::string& id)
    {
    if(varDefs.count(id) != 0)
        return true;
    else
        return constants.exists(id);
    }

void Scope::setVariables(const std::map<std::string, Value>& variables)
    {
    this->variables = variables;
    }

void Scope::AddLocalFunction(FunctionDefExpr& f, CodeBlock&& codeBlock) 
    {
    std::string id = codeBlock.scope->getText(f.idRange);
    localFunctions.emplace(id, new CustomFunction(f, std::move(codeBlock)));
    }

ConstantsView::ConstantsView(Globals& globals)
    : globals(globals)
    {
    constants.insert("PI");
    }

Value& ConstantsView::get(const std::string& id)
    {
    if(exists(id))
        return globals.constants[id];
    throw "blak";
    }
