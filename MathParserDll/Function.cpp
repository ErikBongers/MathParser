#include "pch.h"
#include "Function.h"
#include "Tokenizer.h"

std::map<std::string, Function*> Function::functions;

void Function::init()
    {
    Function::AddFunction(new Sin());
    Function::AddFunction(new Cos());
    Function::AddFunction(new Tan());
    Function::AddFunction(new ArcSin());
    Function::AddFunction(new ArcCos());
    Function::AddFunction(new ArcTan());
    Function::AddFunction(new ASin());
    Function::AddFunction(new ACos());
    Function::AddFunction(new ATan());
    Function::AddFunction(new Sqrt());
    };

Value Sin::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.type == TokenType::DEG)
        arg = arg * M_PI / 180;
    return sin(arg);
    }

Value Cos::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.type == TokenType::DEG)
        arg = arg * M_PI / 180;
    return cos(arg);
    }

Value Tan::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    if (args[0].unit.type == TokenType::DEG)
        arg = arg * M_PI / 180;
    return tan(arg);
    }

Value ArcSin::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return asin(arg);
    }

Value ArcCos::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return acos(arg);
    }

Value ATan::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return atan(arg);
    }

Value ASin::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return asin(arg);
    }

Value ACos::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return acos(arg);
    }

Value ArcTan::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    double arg = args[0].number;
    return atan(arg);
    }

Value Sqrt::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    return sqrt(args[0].number);
    }

bool Function::exists(const std::string& functionName)
    {
    if (functions.size() == 0)
        Function::init();
    return functions.count(functionName) != 0;
    }

Function* Function::get(const std::string& name) 
    {
    if (functions.size() == 0)
        Function::init();
    if (functions.count(name) == 0)
        return nullptr;
    return functions[name]; 
    }
