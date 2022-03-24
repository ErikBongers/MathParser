#include "pch.h"
#include "Function.h"
#include "Tokenizer.h"
#include "Unit.h"
#include "Resolver.h"

void FunctionDefs::init()
    {
    Add(new Int(*this));
    Add(new Abs(*this));
    Add(new Max(*this));
    Add(new Min(*this));
    Add(new Sin(*this));
    Add(new Cos(*this));
    Add(new Tan(*this));
    Add(new ArcSin(*this));
    Add(new ArcCos(*this));
    Add(new ArcTan(*this));
    Add(new ASin(*this));
    Add(new ACos(*this));
    Add(new ATan(*this));
    Add(new Sqrt(*this));
    Add(new Inc(*this));
    Add(new Dec(*this));
    Add(new Round(*this));
    Add(new Floor(*this));
    Add(new Ceil(*this));
    Add(new Trunc(*this));
    };

FunctionDef::FunctionDef(FunctionDefs& functionDefs, const std::string& name, size_t minArgs, size_t maxArgs)
    : name(name), minArgs(minArgs), maxArgs(maxArgs), functionDefs(functionDefs)
    {}

bool FunctionDef::isCorrectArgCount(size_t argCnt)
    {
    return argCnt >= minArgs && argCnt <= maxArgs;
    }

Value FunctionDef::call(std::vector<Value>& args, unsigned int line, unsigned int pos)
    { 
    if (!isCorrectArgCount(args.size()))
        return Value(Number(std::numeric_limits<double>::quiet_NaN(), 0, line, pos));

    return execute(args, line, pos); 
    }

bool FunctionDefs::exists(const std::string& functionName)
    {
    return functions.count(functionName) != 0;
    }

FunctionDef* FunctionDefs::get(const std::string& name)
    {
    if (functions.count(name) == 0)
        return nullptr;
    return functions[name];
    }

Value minMax(FunctionDefs& functionDefs, std::vector<Value>& args, unsigned int line, unsigned int pos, bool max)
    {
    bool diffUnits = false;
    double val0;

    Value ret = args[0];
    auto unit = args[0].number.unit;
    for(int i = 1; i < args.size(); i++)
        {
        val0 = ret.number.toSI(functionDefs.unitDefs);
        double val1 = args[i].number.toSI(functionDefs.unitDefs);
        auto otherErrs = &ret.errors;
        if (max? (val0 > val1) : (val0 < val1))
            {
            //ret keeps it's value.
            }
        else
            {
            ret = args[i];
            }
        if(!functionDefs.unitDefs.isSameProperty(unit, args[i].number.unit))
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
    return minMax(functionDefs, args, line, pos, true);
    }

Value Min::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return minMax(functionDefs, args, line, pos, false);
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
    if (args[0].number.unit.id == "deg")
        arg = functionDefs.unitDefs.get("deg").toSI(arg);;
    return Value(Number(sin(arg), 0, line, pos));
    }

Value Cos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    if (args[0].number.unit.id == "deg")
        arg = functionDefs.unitDefs.get("deg").toSI(arg);
    return Value(Number(cos(arg), 0, line, pos));
    }

Value Tan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    if (args[0].number.unit.id == "deg")
        arg = functionDefs.unitDefs.get("deg").toSI(arg);
    return Value(Number(tan(arg), 0, line, pos));
    }

Value ArcSin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(asin(arg), 0, line, pos));
    }

Value ArcCos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(acos(arg), 0, line, pos));
    }

Value ATan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(atan(arg), 0, line, pos));
    }

Value ASin::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(asin(arg), 0, line, pos));
    }

Value ACos::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(acos(arg), 0, line, pos));
    }

Value ArcTan::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    double arg = args[0].number.to_double();
    return Value(Number(atan(arg), 0, line, pos));
    }

Value Sqrt::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(sqrt(args[0].number.to_double()), 0, line, pos));
    }

Value Round::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(round(args[0].number.to_double()), 0, line, pos));
    }

Value Floor::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(floor(args[0].number.to_double()), 0, line, pos));
    }

Value Ceil::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(ceil(args[0].number.to_double()), 0, line, pos));
    }

Value Trunc::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(trunc(args[0].number.to_double()), 0, line, pos));
    }



