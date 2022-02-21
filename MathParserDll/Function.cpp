#include "pch.h"
#include "Function.h"
#include "Tokenizer.h"
#include "Unit.h"

std::map<std::string, FunctionDef*> FunctionDef::functions;

void FunctionDef::init()
    {
    FunctionDef::AddFunction(new Int());
    FunctionDef::AddFunction(new Abs());
    FunctionDef::AddFunction(new Max());
    FunctionDef::AddFunction(new Sin());
    FunctionDef::AddFunction(new Cos());
    FunctionDef::AddFunction(new Tan());
    FunctionDef::AddFunction(new ArcSin());
    FunctionDef::AddFunction(new ArcCos());
    FunctionDef::AddFunction(new ArcTan());
    FunctionDef::AddFunction(new ASin());
    FunctionDef::AddFunction(new ACos());
    FunctionDef::AddFunction(new ATan());
    FunctionDef::AddFunction(new Sqrt());
    FunctionDef::AddFunction(new Inc());
    FunctionDef::AddFunction(new Dec());
    };

bool FunctionDef::exists(const std::string& functionName)
    {
    if (functions.size() == 0)
        FunctionDef::init();
    return functions.count(functionName) != 0;
    }

FunctionDef* FunctionDef::get(const std::string& name)
    {
    if (functions.size() == 0)
        FunctionDef::init();
    if (functions.count(name) == 0)
        return nullptr;
    return functions[name];
    }

// --- Standard function implementations ---

Value Max::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 2)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg0 = args[0].toSI();
    double arg1 = args[1].toSI();
    Value ret;
    auto otherErrs = &ret.errors;
    if (arg0 > arg1)
        {
        ret = args[0];
        otherErrs = &args[1].errors;
        }
    else
        {
        ret = args[1];
        otherErrs = &args[0].errors;
        }
    ret.errors.insert(ret.errors.begin(), otherErrs->begin(), otherErrs->end());
    return ret;
    }

Value Inc::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    Value arg = args[0];
    arg.number++;
    return arg;
    }

Value Dec::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    Value arg = args[0];
    arg.number--;
    return arg;
    }

Value Int::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    Value arg = args[0];
    arg.number = trunc(arg.number);
    return arg;
    }

Value Abs::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    Value arg = args[0];
    arg.number = abs(arg.number);
    return arg;
    }

Value Sin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    if (args[0].unit.id == "deg")
        arg = UnitDef::defs["deg"].toSI(arg);;
    return Value(sin(arg), line, pos);
    }

Value Cos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    if (args[0].unit.id == "deg")
        arg = UnitDef::defs["deg"].toSI(arg);
    return Value(cos(arg), line, pos);
    }

Value Tan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    if (args[0].unit.id == "deg")
        arg = UnitDef::defs["deg"].toSI(arg);
    return Value(tan(arg), line, pos);
    }

Value ArcSin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(asin(arg), line, pos);
    }

Value ArcCos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(acos(arg), line, pos);
    }

Value ATan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(atan(arg), line, pos);
    }

Value ASin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(asin(arg), line, pos);
    }

Value ACos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(acos(arg), line, pos);
    }

Value ArcTan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    double arg = args[0].number;
    return Value(atan(arg), line, pos);
    }

Value Sqrt::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    if (args.size() != 1)
        return Value(std::numeric_limits<double>::quiet_NaN(), line, pos);
    return Value(sqrt(args[0].number), line, pos);
    }

