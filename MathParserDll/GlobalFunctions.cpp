#include "pch.hpp"
#include "GlobalFunctions.h"
#include <chrono>
#include "Globals.h"
#include "Scope.h"

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

std::vector<Value>* explodeArgs(std::vector<Value>& args, std::vector<Value>& emptyExplodedList)
    {
    std::vector<Value>* argsPtr = &args;

    if (args[0].type == ValueType::LIST)
        {
        //explode args[0]
        for (auto &arg : args[0].getList().numberList)
            emptyExplodedList.push_back(arg);
        argsPtr = &emptyExplodedList;
        }
    return argsPtr;
    }

Value minMax(Globals& globals, std::vector<Value>& args, const Range& range, bool max)
    {
    bool diffUnits = false;
    Number val0;
    std::vector<Value> explodedList;
    auto& argList = *explodeArgs(args, explodedList);
    Value ret = argList[0];
    auto unit = argList[0].getNumber().unit;
    for(int i = 1; i < argList.size(); i++)
        {
        val0 = ret.getNumber().toSI(globals.unitsView);
        Number val1 = argList[i].getNumber().toSI(globals.unitsView);
        auto otherErrs = &ret.errors;
        if (max? (val0 > val1) : (val0 < val1))
            {
            //ret keeps it's value.
            }
        else
            {
            ret = argList[i];
            }
        if(!globals.unitsView.isSameProperty(unit, argList[i].getNumber().unit))
            diffUnits = true;
        }
    std::vector<Error> errors;
    for(auto& arg : argList)
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


Value Abs::execute(std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber ()= {abs(arg.getNumber().to_double()), 0, range};
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
    return Value(Number(round(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Floor::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(floor(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Ceil::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(ceil(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Trunc::execute(std::vector<Value>& args, const Range& range)
    {
    return Value(Number(trunc(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Factors::execute(std::vector<Value>& args, const Range& range)
    {

    long long number = (long long) args[0].getNumber().to_double();
    long long half = number/2;
    std::vector<Number> numberList;
    for(int i = 2; i <= half; i++)
        {
        if(number%i == 0)
            {
            numberList.push_back(Number(i, 0, range));
            }
        }

    return Value(numberList);
    }

Value DateFunc::execute(std::vector<Value>& args, const Range& range)
    {
    //first get indexes for the current date format.
    int iDay = 0, iMonth = 0, iYear = 0;

    DateFormat dateFormat = DateFormat::UNDEFINED;
    switch (dateFormat)
        {
        using enum DateFormat;
        case DMY:
            iDay = 0; iMonth = 1; iYear = 2; break;
        case MDY:
            iMonth = 0; iDay = 1; iYear = 2; break;
        case YMD:
        case UNDEFINED: //fall through
            iYear = 0; iMonth = 1; iDay = 2; break;
        }

    Number day;
    Number month;
    Number year;
    Range rDay;
    
    std::vector<Value> explodedList;
    auto& argList = *explodeArgs(args, explodedList);

    //just a minimal implementation for now...
    if(argList.size() != 3)
        return Value();
    if(!argList[0].isNumber()) //TODO: also test the other values.
        return Value(Error(ErrorId::INV_DATE_VALUE, day.range)); //TODO: proper error message and args.
    day = argList[iDay].getNumber();
    month = argList[iMonth].getNumber();
    year = argList[iYear].getNumber();

    Date date;

    if(day.to_double() < 1 || day.to_double() > 31)// don't check date.day as it might have been truncated!
        return Value(Error(ErrorId::INV_DATE_VALUE, day.range, std::to_string(day.to_double()), "day"));
    if(month.to_double() < 1 || month.to_double() > 12)
        return Value(Error(ErrorId::INV_DATE_VALUE, month.range, std::to_string(month.to_double()), "month"));

    date.day = (char)day.to_double();
    date.month = (Month)month.to_double();
    date.year = (long)year.to_double();

    date.errors.insert(date.errors.end(), day.errors.begin(), day.errors.end());
    date.errors.insert(date.errors.end(), month.errors.begin(), month.errors.end());
    date.errors.insert(date.errors.end(), year.errors.begin(), year.errors.end());
    return Value(date);

    }

bool compareNumber(Number n1, Number n2)
    {
    return (n1.to_double() < n2.to_double());
    }

Value Sort::execute(std::vector<Value>& args, const Range& range)
    {
    std::vector<Value> explodedList;
    auto& argList = *explodeArgs(args, explodedList);
    std::vector<Number> sortedList;
    for (auto &val : argList)
        {
        if(val.isNumber())
            sortedList.push_back(val.getNumber());
        }
    sort(sortedList.begin(), sortedList.end(), compareNumber);
    return Value(sortedList);
    }

Value Reverse::execute(std::vector<Value>& args, const Range& range)
    {
    std::vector<Value> explodedList;
    auto& argList = *explodeArgs(args, explodedList);
    std::vector<Number> reverseList;
    for (auto &val : argList)
        {
        if(val.isNumber())
            reverseList.push_back(val.getNumber());
        }
    reverse(reverseList.begin(), reverseList.end());
    return Value(reverseList);
    }

Value Factorial::execute(std::vector<Value>& args, const Range& range)
    {
    auto size = args[0].getNumber().to_double();
    double val = 1;
    //TODO: check if size is int, positive and not larger than....10?
    for (int i = 1; i <= size; i++)
        {
        val *= i;
        }

    return Value(Number(val, 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

