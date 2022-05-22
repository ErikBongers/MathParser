#pragma once
#include <vector>
#include "Value.h"
#include "Date.h"
#include "Unit.h"
#include "Function.h"
#include "Variable.h"

struct Globals;
class FunctionDefExpr;

class Scope
    {
    public:
        Globals& globals;
        Scope* parentScope = nullptr;
        //views (inherited)
        UnitsView units;
        //settings
        DateFormat dateFormat = DateFormat::UNDEFINED;
        //locals
        std::map<std::string, Variable> ids; //TODO: rename to varDefs
        std::map<std::string, Value> variables; //TODO: rename to varValues
        std::map<std::string, CustomFunction*> localFunctions;

        Scope(Globals& globals);
        ~Scope();
        Scope* copyForScript();
        Scope* copyForFunction();
        void AddLocalFunction(FunctionDefExpr& f, Scope* scope);
        bool functionExists(const std::string name);
        FunctionDef* getFunction(const std::string& name);
    private:
        CustomFunction* getLocalFunction(const std::string& name);
        Scope(const Scope& scope) = default;
    };