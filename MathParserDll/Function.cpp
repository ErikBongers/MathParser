#include "pch.h"
#include "Function.h"
#include "Tokenizer.h"

std::map<std::string, FunctionDef*> FunctionDef::functions;

void FunctionDef::init()
    {
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

Value Max::execute(std::vector<Value>& args)
    {
    if (args.size() != 2)
        return std::numeric_limits<double>::quiet_NaN();
    double arg0 = args[0].number;
    if (args[0].unit.id.stringValue == "deg")
        arg0 = arg0 * M_PI / 180;
    double arg1 = args[1].number;
    if (args[1].unit.id.stringValue == "deg")
        arg1 = arg1 * M_PI / 180;
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

Value Sin::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.id.stringValue == "deg")
        arg = arg * M_PI / 180;
    return sin(arg);
    }

Value Cos::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.id.stringValue == "deg")
        arg = arg * M_PI / 180;
    return cos(arg);
    }

Value Tan::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.id.stringValue == "deg")

        arg = arg * M_PI / 180;
    return tan(arg);
    }

Value ArcSin::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return asin(arg);
    }

Value ArcCos::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return acos(arg);
    }

Value ATan::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return atan(arg);
    }

Value ASin::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return asin(arg);
    }

Value ACos::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return acos(arg);
    }

Value ArcTan::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return atan(arg);
    }

Value Sqrt::execute(std::vector<Value>& args)
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    return sqrt(args[0].number);
    }

