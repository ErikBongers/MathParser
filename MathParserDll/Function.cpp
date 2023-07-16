#include "pch.hpp"
#include "Function.h"
#include "Scope.h"
#include "Globals.h"
#include "Tokenizer.h"
#include "Resolver.h"
#include "Date.h"
#include "GlobalFunctions.h"
#include "FunctionsView.h"

void FunctionDefs::init()
    {
    Add(new Now());
    Add(new Abs());
    Add(new Max());
    Add(new Min());
    Add(new Sin());
    Add(new Cos());
    Add(new Tan());
    Add(new ArcSin());
    Add(new ArcCos());
    Add(new ArcTan());
    Add(new ASin());
    Add(new ACos());
    Add(new ATan());
    Add(new Sqrt());
    Add(new Inc());
    Add(new Dec());
    Add(new Round());
    Add(new Floor());
    Add(new Ceil());
    Add(new Trunc());
    Add(new Factors());
    Add(new DateFunc());
    Add(new Sort());
    Add(new Reverse());
    Add(new Factorial());
    };

FunctionDef::FunctionDef(const std::string& name, size_t minArgs, size_t maxArgs)
    : name(name), minArgs(minArgs), maxArgs(maxArgs)
    {}

bool FunctionDef::isCorrectArgCount(size_t argCnt)
    {
    return argCnt >= minArgs && argCnt <= maxArgs;
    }

Value FunctionDef::call(Scope& scope, std::vector<Value>& args, const Range& range)
    { 
    if (!isCorrectArgCount(args.size()))
        return Value(Number(std::numeric_limits<double>::quiet_NaN(), 0, range));

    return execute(scope, args, range); 
    }

bool FunctionDefs::exists(const std::string& functionName)
    {
    return functions.count(functionName) != 0;
    }

void FunctionDefs::Add(FunctionDef* f) { functions.emplace(f->name, f); }

FunctionDef* FunctionDefs::get(const std::string& name)
    {
    if (functions.count(name) == 0)
        return nullptr;
    return functions.find(name)->second;
    }

CustomFunction::CustomFunction(FunctionDefExpr& functionDefExpr, CodeBlock&& codeBlock)
    : FunctionDef(functionDefExpr.id, functionDefExpr.params.size(), functionDefExpr.params.size()), functionDefExpr(functionDefExpr), codeBlock(std::move(codeBlock))
    {
    }


Value CustomFunction::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    std::map<std::string, Value> paramVariables;

    //args.size and params.size should be equal.
    for (size_t i = 0; i < args.size(); i++)
        {
        paramVariables.emplace(functionDefExpr.params[i], args[i]);
        }
    codeBlock.scope->setVariables(paramVariables);
    Resolver resolver(codeBlock);
    return resolver.resolveFunctionBlock(range,functionDefExpr.id);
    }

std::vector<Error> CustomFunction::getErrors()
    {
    std::vector<Error> errors;
    for (auto& stmt : codeBlock.statements)
        {    
        if (stmt->error.id != ErrorId::NONE)
            errors.push_back(stmt->error);
        }
    if(!functionDefExpr.error.isNone())
        errors.push_back(functionDefExpr.error);
    return errors;
    }

std::vector<std::string> FunctionKeys::trig = { "sin", "cos", "tan", "asin", "acos", "atan", "arcsin", "arccos", "arctan"};
std::vector<std::string> FunctionKeys::arithm = { "round", "trunc", "floor", "ceil", "abs", "max", "min"};
std::vector<std::string> FunctionKeys::date = { "now", "date"};

const std::vector<std::string>& FunctionsView::getFuncKeyList(FunctionType type)
    {
    switch (type)
        {
        using enum FunctionType;
        case TRIG: return functionKeys.trig;
        case ARITHM: return functionKeys.arithm;
        case DATE: return functionKeys.all;
        case ALL: return functionKeys.all;
        default: return  functionKeys.all; //should not happen.
        }
    }
;void FunctionsView::addFunctions(FunctionType type)
    {
    for(auto key: getFuncKeyList(type))
        defs.insert(key);
    }

void FunctionsView::removeFunctions(FunctionType type)
    {
    for(auto key: getFuncKeyList(type))
        defs.erase(key);
    }

FunctionsView::FunctionsView(Globals& globals) 
    : globals(globals) 
    {
    for (auto& func : globals.functionDefs.functions)
        {
        defs.insert(func.first);
        }
    }

FunctionDef* FunctionsView::get(const std::string& key)
    {
    if(exists(key))
        return globals.functionDefs.functions[key];
    else
        return nullptr;
    }

FunctionKeys FunctionsView::functionKeys;

FunctionKeys::FunctionKeys()
    {
    all.insert(all.end(), trig.begin(), trig.end());
    all.insert(all.end(), arithm.begin(), arithm.end());
    all.insert(all.end(), date.begin(), date.end());
    }
