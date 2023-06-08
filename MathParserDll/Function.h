#pragma once
#include <vector>
#include "Value.h"
#include "Unit.h"
#include <string>
#include <map>
#include <memory>
#include "CodeBlock.h"

struct Globals;
class FunctionsView;
class FunctionDef
    {
    public:
        std::string name;
        size_t minArgs;
        size_t maxArgs;

        FunctionDef(const std::string& name, size_t minArgs, size_t maxArgs);

        bool isCorrectArgCount(size_t argCnt);
        Value call(Scope& scope, std::vector<Value>& args, const Range& range);
        virtual std::vector<Error> getErrors() { return std::vector<Error>(); }
    private:
        virtual Value execute(Scope& scope, std::vector<Value>& args, const Range& range) = 0;
    };

class FunctionDefs
    {
    protected:
        std::map<std::string, FunctionDef*> functions;

    public:
        FunctionDefs() { init(); }
        void init();
        bool exists(const std::string& functionName);
        void Add(FunctionDef* f);
        FunctionDef* get(const std::string& name);
        friend class FunctionsView;
    };

class Statement;
class OperatorDefs;
class FunctionDefExpr;
class CustomFunction: public FunctionDef
    {
    private:
        FunctionDefExpr& functionDefExpr;

    public:
        CodeBlock codeBlock;
        CustomFunction(FunctionDefExpr& functionDefExpr, CodeBlock&& codeBlock);
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
        virtual std::vector<Error> getErrors() override;
    };
