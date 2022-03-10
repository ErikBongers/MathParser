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
    FunctionDef::AddFunction(new Min());
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
    FunctionDef::AddFunction(new Round());
    FunctionDef::AddFunction(new Floor());
    FunctionDef::AddFunction(new Ceil());
    FunctionDef::AddFunction(new Trunc());
    };

bool FunctionDef::isCorrectArgCount(size_t argCnt)
    {
    return argCnt >= minArgs() && argCnt <= maxArgs();
    }

Value Function::execute(unsigned int line, unsigned int pos)
    { 
    if (!functionDef.isCorrectArgCount(args.size()))
        return Value({std::numeric_limits<double>::quiet_NaN(), 0}, line, pos);

    return functionDef.execute(args, line, pos); 
    }

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

Value minMax(std::vector<Value>& args, unsigned int line, unsigned int pos, bool max)
    {
    bool diffUnits = false;
    Value ret;
    double val0;

    ret = args[0];
    auto unit = args[0].unit;
    for(int i = 1; i < args.size(); i++)
        {
        val0 = ret.toSI();
        double val1 = args[i].toSI();
        auto otherErrs = &ret.errors;
        if (max? (val0 > val1) : (val0 < val1))
            {
            //ret keeps it's value.
            }
        else
            {
            ret = args[i];
            }
        if(!UnitDef::isSameProperty(unit, args[i].unit))
           diffUnits = true;
        }
    std::vector<Error> errors;
    for(auto& arg : args)
        {
        errors.insert(errors.begin(), arg.errors.begin(), arg.errors.end());
        }
    if(diffUnits)
        errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
    ret.errors = errors;
    return ret;
    }

Value Max::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return minMax(args, line, pos, true);
    }

Value Min::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return minMax(args, line, pos, false);
    }

Value Inc::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value arg = args[0];
    arg.number++;
    return arg;
    }

Value Dec::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value arg = args[0];
    arg.number--;
    return arg;
    }

Value Int::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value arg = args[0];
    arg.number = {trunc(arg.number.to_double()), 0};
    return arg;
    }

Value Abs::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value arg = args[0];
    arg.number = {abs(arg.number.to_double()), 0};
    return arg;
    }

Value Sin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    if (args[0].unit.id.stringValue == "deg")
        arg = UnitDef::get("deg").toSI(arg);;
    return Value({sin(arg), 0}, line, pos);
    }

Value Cos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    if (args[0].unit.id.stringValue == "deg")
        arg = UnitDef::get("deg").toSI(arg);
    return Value({cos(arg), 0}, line, pos);
    }

Value Tan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    if (args[0].unit.id.stringValue == "deg")
        arg = UnitDef::get("deg").toSI(arg);
    return Value({tan(arg), 0}, line, pos);
    }

Value ArcSin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({asin(arg), 0}, line, pos);
    }

Value ArcCos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({acos(arg), 0}, line, pos);
    }

Value ATan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({atan(arg), 0}, line, pos);
    }

Value ASin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({asin(arg), 0}, line, pos);
    }

Value ACos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({acos(arg), 0}, line, pos);
    }

Value ArcTan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value({atan(arg), 0}, line, pos);
    }

Value Sqrt::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value({sqrt(args[0].number.to_double()), 0}, line, pos);
    }

Value Round::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value({round(args[0].number.to_double()), 0}, line, pos);
    }

Value Floor::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value({floor(args[0].number.to_double()), 0}, line, pos);
    }

Value Ceil::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value({ceil(args[0].number.to_double()), 0}, line, pos);
    }

Value Trunc::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value({trunc(args[0].number.to_double()), 0}, line, pos);
    }



