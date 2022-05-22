#include "pch.hpp"
#include "Scope.h"
#include "Globals.h"
#include "Unit.h"
#include "Function.h"
#include "Parser.h"

Scope::Scope(Globals& globals) 
    : globals(globals), units(globals)
    {
    }

Scope::~Scope()
    {
    for(auto& f : localFunctions)
        delete f.second;
    }

Scope* Scope::copyForScript()
    {
    auto newScope = new Scope(globals);
    newScope->parentScope = this;
    return newScope; 
    }

Scope* Scope::copyForFunction()
    {
    auto newScope = new Scope(globals);
    newScope->parentScope = this;
    newScope->units.setDefs(units);
    //settings;
    newScope->dateFormat = dateFormat;
    //don't copy variables

    return newScope;
    }

bool Scope::functionExists(const std::string name)
    {
    if(localFunctions.count(name) != 0)
        return true;
    else if(parentScope != nullptr && parentScope->functionExists(name))
        return true;
    else 
        return globals.functionDefs.exists(name);
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
    return globals.functionDefs.get(name);
    }

void Scope::AddLocalFunction(FunctionDefExpr& f, Scope* scope) 
    { 
    localFunctions.emplace(f.id.stringValue, new CustomFunction(f, scope));
    }

