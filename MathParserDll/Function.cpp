#include "pch.h"
#include "Function.h"
#include "Tokenizer.h"

std::map<std::string, Function*> Function::functions;

void Function::init()
    {
    Function::AddFunction(new Sin());
    Function::AddFunction(new Cos());
    //Function::AddFunction(new Tan());
    //Function::AddFunction(new ArcSin());
    //Function::AddFunction(new ArcCos());
    //Function::AddFunction(new ArcTan());
    //Function::AddFunction(new Asin());
    //Function::AddFunction(new Acos());
    //Function::AddFunction(new Atan());
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

Value Sqrt::execute()
    {
    if (args.size() != 1)
        return std::numeric_limits<double>::quiet_NaN();
    return sqrt(args[0].number);
    }

Function* Function::get(const std::string& name) 
    {
    if (functions.size() == 0)
        Function::init();
    if (functions.count(name) == 0)
        return nullptr;
    return functions[name]; 
    }
