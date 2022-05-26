#include "pch.hpp"
#include "Function.h"
#include "Scope.h"
#include "Globals.h"
#include "Tokenizer.h"
#include "Resolver.h"
#include "Date.h"
#include "GlobalFunctions.h"

void FunctionDefs::init()
    {
    Add(new Now(globals));
    Add(new Int(globals));
    Add(new Abs(globals));
    Add(new Max(globals));
    Add(new Min(globals));
    Add(new Sin(globals));
    Add(new Cos(globals));
    Add(new Tan(globals));
    Add(new ArcSin(globals));
    Add(new ArcCos(globals));
    Add(new ArcTan(globals));
    Add(new ASin(globals));
    Add(new ACos(globals));
    Add(new ATan(globals));
    Add(new Sqrt(globals));
    Add(new Inc(globals));
    Add(new Dec(globals));
    Add(new Round(globals));
    Add(new Floor(globals));
    Add(new Ceil(globals));
    Add(new Trunc(globals));
    };

FunctionDef::FunctionDef(Globals& globals, const std::string& name, size_t minArgs, size_t maxArgs)
    : globals(globals), name(name), minArgs(minArgs), maxArgs(maxArgs)
    {}

bool FunctionDef::isCorrectArgCount(size_t argCnt)
    {
    return argCnt >= minArgs && argCnt <= maxArgs;
    }

Value FunctionDef::call(std::vector<Value>& args, const Range& range)
    { 
    if (!isCorrectArgCount(args.size()))
        return Value(Number(std::numeric_limits<double>::quiet_NaN(), 0, range));

    return execute(args, range); 
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
    : FunctionDef(codeBlock.scope->globals, functionDefExpr.id.stringValue, functionDefExpr.params.size(), functionDefExpr.params.size()), functionDefExpr(functionDefExpr), codeBlock(std::move(codeBlock))
    {
    }


Value CustomFunction::execute(std::vector<Value>& args, const Range& range)
    {
    std::map<std::string, Value> paramVariables;

    //args.size and params.size should be equal.
    for (size_t i = 0; i < args.size(); i++)
        {
        paramVariables.emplace(functionDefExpr.params[i].stringValue, args[i]);
        }
    codeBlock.scope->variables = paramVariables;
    Resolver resolver(codeBlock);
    return resolver.resolveBlock(range,functionDefExpr.id.stringValue);
    }

std::vector<std::string> FunctionKeys::trig = { "sin", "cos", "tan", "asin", "acos", "atan", "arcsin", "arccos", "arctan"};
std::vector<std::string> FunctionKeys::arithm = { "round", "trunc", "floor", "ceil", "abs", "max", "min"};
std::vector<std::string> FunctionKeys::date = { "now"};

const std::vector<std::string>& FunctionsView::getFuncKeyList(FunctionType type)
    {
    switch (type)
        {
        using enum FunctionType;
        case TRIG: return functionKeys.trig;
        case ARITHM: return functionKeys.arithm;
        case DATE: return functionKeys.all;
        case ALL: return functionKeys.all;
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
