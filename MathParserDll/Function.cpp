#include "pch.hpp"
#include "Function.h"
#include "Scope.h"
#include "Globals.h"
#include "Tokenizer.h"
#include "Resolver.h"
#include <chrono>
#include "Date.h"

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

CustomFunction::CustomFunction(FunctionDefExpr& functionDefExpr, std::unique_ptr<Scope>&& scope)
    : FunctionDef(scope->globals, functionDefExpr.id.stringValue, functionDefExpr.params.size(), functionDefExpr.params.size()), functionDefExpr(functionDefExpr), codeBlock(std::move(scope))
    {
    codeBlock.statements = std::move(functionDefExpr.statements);
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

namespace c = std::chrono;
using time_point = c::system_clock::time_point;
using year_month_day = c::year_month_day;
using namespace std::chrono_literals;

Value Now::execute(std::vector<Value>& args, const Range& range)
    {
    Value now;
    time_point tpNow = c::system_clock::now();
    year_month_day ymd{ c::floor<std::chrono::days>(tpNow) };
    Date date;
    date.day = static_cast<unsigned>(ymd.day());
    date.month = (Month)static_cast<unsigned>(ymd.month());
    date.year =  static_cast<int>(ymd.year());
    now.setDate(date);
    return now;
    }

Value minMax(Globals& globals, std::vector<Value>& args, const Range& range, bool max)
    {
    bool diffUnits = false;
    double val0;

    Value ret = args[0];
    auto unit = args[0].getNumber().unit;
    for(int i = 1; i < args.size(); i++)
        {
        val0 = ret.getNumber().toSI(globals.unitsView);
        double val1 = args[i].getNumber().toSI(globals.unitsView);
        auto otherErrs = &ret.errors;
        if (max? (val0 > val1) : (val0 < val1))
            {
            //ret keeps it's value.
            }
        else
            {
            ret = args[i];
            }
        if(!globals.unitsView.isSameProperty(unit, args[i].getNumber().unit))
           diffUnits = true;
        }
    std::vector<Error> errors;
    for(auto& arg : args)
        {
        errors.insert(errors.begin(), arg.errors.begin(), arg.errors.end());
        }
    if(diffUnits)
        errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, range));
    ret.errors = errors;
    return ret;
    }

Value Max::execute(std::vector<Value>& args, const Range& range)
    {
    return minMax(globals, args, range, true);
    }

Value Min::execute(std::vector<Value>& args, const Range& range)
    {
    return minMax(globals, args, range, false);
    }

Value Inc::execute(std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber()++;
    return arg;
    }

Value Dec::execute(std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber()--;
    return arg;
    }

Value Int::execute(std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber ()= {trunc(arg.getNumber().to_double()), 0};
    return arg;
    }

Value Abs::execute(std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber ()= {abs(arg.getNumber().to_double()), 0};
    return arg;
    }

Value Sin::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = globals.unitsView.get("deg").toSI(arg);;
    return Value(Number(sin(arg), 0, range));
    }

Value Cos::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = globals.unitsView.get("deg").toSI(arg);
    return Value(Number(cos(arg), 0, range));
    }

Value Tan::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = globals.unitsView.get("deg").toSI(arg);
    return Value(Number(tan(arg), 0, range));
    }

Value ArcSin::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(asin(arg), 0, range));
    }

Value ArcCos::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(acos(arg), 0, range));
    }

Value ATan::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(atan(arg), 0, range));
    }

Value ASin::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(asin(arg), 0, range));
    }

Value ACos::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(acos(arg), 0, range));
    }

Value ArcTan::execute(std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(atan(arg), 0, range));
    }

Value Sqrt::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(sqrt(args[0].getNumber().to_double()), 0, range));
    }

Value Round::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(round(args[0].getNumber().to_double()), 0, range));
    }

Value Floor::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(floor(args[0].getNumber().to_double()), 0, range));
    }

Value Ceil::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(ceil(args[0].getNumber().to_double()), 0, range));
    }

Value Trunc::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(trunc(args[0].getNumber().to_double()), 0, range));
    }



