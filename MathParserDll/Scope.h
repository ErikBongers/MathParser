#pragma once
#include <vector>
#include "Value.h"
#include "Date.h"
#include "Unit.h"
#include "Variable.h"
#include <memory>
#include "Function.h"
#include "FunctionsView.h"

struct Globals;
struct CodeBlock;
class Scope;
class Value;
class CustomFunction;

class ConstantsView
    {
    Globals& globals;
    std::set<std::string> constants;
    public:
        ConstantsView(Globals& globals);
        bool exists(const std::string& id) { return constants.count(id) != 0; }
        Value& get(const std::string& id);
    };

class Scope
    {
    public:
        Globals& globals;
        Scope* parentScope = nullptr;
        //views (inherited)
        UnitsView units;
        FunctionsView functions;
        ConstantsView constants;
        //settings
        DateFormat dateFormat = DateFormat::UNDEFINED;
        std::map<std::string, CustomFunction*> localFunctions;
        bool strict = false;
        char decimal_char = '.';
        char thou_char = ',';

        Scope(Globals& globals);
        ~Scope();
        std::unique_ptr<Scope> copyForScript();
        std::unique_ptr<Scope> copyForBlock();
        void AddLocalFunction(FunctionDefExpr& f, CodeBlock&& codeBlock);
        bool functionExists(const std::string name) const;
        FunctionDef* getFunction(const std::string& name);
        Value& getVariable(const std::string& id);
        bool varExists(const std::string& id);
        bool varDefExists(const std::string& id);
        void setVariables(const std::map<std::string, Value>& variables);
        void emplaceVarDef(const std::string& id, Variable&& variable) { varDefs.emplace(id, std::move(variable)); }
        void emplaceVariable(const std::string& id, Value&& value) { variables.emplace(id, std::move(value)); }
        std::string getText(char sourceIndex, unsigned int start, unsigned end);
        std::string getText(const Range& range);

    private:
        CustomFunction* getLocalFunction(const std::string& name);
        Scope(const Scope& scope) = delete;
        Scope& operator=(const Scope& scope) = delete;
        //locals
        std::map<std::string, Value> variables;
        std::map<std::string, Variable> varDefs;
    };