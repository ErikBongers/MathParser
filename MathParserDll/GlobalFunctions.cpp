#include "pch.hpp"
#include "GlobalFunctions.h"
#include <chrono>
#include "Globals.h"
#include "Scope.h"
#include <algorithm>

namespace c = std::chrono;
using time_point = c::system_clock::time_point;
using year_month_day = c::year_month_day;
using namespace std::chrono_literals;

Value Now::execute(Scope& scope, std::vector<Value>& args, const Range& range)
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

void appendToList(Value arg, std::vector<Value>& explodedList)
    {
    if (arg.type == ValueType::LIST)
        {
        for (auto &a : arg.getList().numberList)
            explodedList.push_back(a);
        }
    else
        explodedList.push_back(arg);
    }

std::vector<Value>& explodeArgs(std::vector<Value>& args, std::vector<Value>& explodedList)
    {
    if(args.size() == 1 && args[0].type != ValueType::LIST)
        return args;
    
    for(auto& arg: args)
        {
        appendToList(arg, explodedList);
        }
    return explodedList;
    }



Value minMax(Scope& scope, std::vector<Value>& args, const Range& range, bool max)
    {
    bool diffUnits = false;
    Number val0;
    std::vector<Value> explodedList;
    auto& argList = explodeArgs(args, explodedList);
    Value ret = argList[0];
    auto unit = argList[0].getNumber().unit;
    for(int i = 1; i < argList.size(); i++)
        {
        val0 = ret.getNumber().toSI(scope.globals.unitsView);
        Number val1 = argList[i].getNumber().toSI(scope.globals.unitsView);
        auto otherErrs = &ret.errors;
        if (max? (val0 > val1) : (val0 < val1))
            {
            //ret keeps it's value.
            }
        else
            {
            ret = argList[i];
            }
        if(!scope.globals.unitsView.isSameProperty(unit, argList[i].getNumber().unit))
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

Value Max::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return minMax(scope, args, range, true);
    }

Value Min::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return minMax(scope, args, range, false);
    }

Value Inc::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber()++;
    return arg;
    }

Value Dec::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber()--;
    return arg;
    }


Value Abs::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    Value arg = args[0];
    arg.getNumber ()= {abs(arg.getNumber().to_double()), 0, range};
    return arg;
    }

Value Sin::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = scope.globals.unitsView.get("deg").toSI(arg);
    auto val = Value(Number(sin(arg), 0, range));
    if (scope.strict && args[0].getNumber().unit.id != "rad" && args[0].getNumber().unit.id != "deg")
        val.errors.push_back(Error(ErrorId::W_EXPLICIT_UNITS_EXPECTED, range, "rad, deg"));
    return val;
    }

Value Cos::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = scope.globals.unitsView.get("deg").toSI(arg);
    auto val = Value(Number(cos(arg), 0, range));
    if (scope.strict && args[0].getNumber().unit.id != "rad" && args[0].getNumber().unit.id != "deg")
        val.errors.push_back(Error(ErrorId::W_EXPLICIT_UNITS_EXPECTED, range, "rad, deg"));
    return val;
    }

Value Tan::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    if (args[0].getNumber().unit.id == "deg")
        arg = scope.globals.unitsView.get("deg").toSI(arg);
    auto val = Value(Number(tan(arg), 0, range));
    if (scope.strict && args[0].getNumber().unit.id != "rad" && args[0].getNumber().unit.id != "deg")
        val.errors.push_back(Error(ErrorId::W_EXPLICIT_UNITS_EXPECTED, range, "rad, deg"));
    return val;
    }

Value ArcSin::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(asin(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value ArcCos::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(acos(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value ATan::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(atan(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value ASin::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(asin(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value ACos::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(acos(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value ArcTan::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double arg = args[0].getNumber().to_double();
    return Value(Number(atan(arg), 0, Unit("rad", range), NumFormat::DEC, range));
    }

Value Sqrt::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(sqrt(args[0].getNumber().to_double()), 0, range));
    }

Value Round::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(round(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Floor::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(floor(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Ceil::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(ceil(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value Trunc::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(trunc(args[0].getNumber().to_double()), 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

Value DateFunc::execute(Scope& scope, std::vector<Value>& args, const Range& range)
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
    auto& argList = explodeArgs(args, explodedList);

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

Value Sort::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    std::vector<Value> explodedList;
    auto& argList = explodeArgs(args, explodedList);
    std::vector<Number> sortedList;
    for (auto &val : argList)
        {
        if(val.isNumber())
            sortedList.push_back(val.getNumber());
        }
    sort(sortedList.begin(), sortedList.end(), compareNumber);
    return Value(sortedList);
    }

Value Reverse::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    std::vector<Value> explodedList;
    auto& argList = explodeArgs(args, explodedList);
    std::vector<Number> reverseList;
    for (auto &val : argList)
        {
        if(val.isNumber())
            reverseList.push_back(val.getNumber());
        }
    reverse(reverseList.begin(), reverseList.end());
    return Value(reverseList);
    }

Value Factorial::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    if(!args[0].isNumber())
        return Value(Error(ErrorId::EXPECTED_NUMERIC_VALUE, args[0].getNumber().range));
    auto number = args[0].getNumber();
    auto size = number.to_double();
    if(size < 0)
        return Value(Error(ErrorId::EXPECTED, number.range, "factorial argument should be a non-negative integer."));
    if(size != (int) size)
        return Value(Error(ErrorId::EXPECTED, number.range, "factorial argument should be an integer value."));

    double val = 1;
    for (int i = 1; i <= size; i++)
        {
        val *= i;
        }

    return Value(Number(val, 0, args[0].getNumber().unit, args[0].getNumber().numFormat, range));
    }

std::vector<long long> factors(long long number)
    {
    long long half = number/2;
    std::vector<long long> numberList;
    for(int i = 2; i <= half; i++)
        {
        if(number%i == 0)
            {
            numberList.push_back(i);
            }
        }

    return numberList;
    }

Value Factors::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    long long number = (long long) args[0].getNumber().to_double();
    auto numberList = factors(number);
    std::vector<Number> numbers;
    for (auto n : numberList)
        {
        numbers.push_back(Number((double)n,0, range));
        }
    return Value(numbers);
    }

static const std::vector<int> primes {
    2,3,5,7,11,13,17,19,23,
    29,31,37,41,43,47,53,59,61,67,
    71,73,79,83,89,97,101,103,107,109,
    113,127,131,137,139,149,151,157,163,167,
    173,179,181,191,193,197,199,211,223,227,
    229,233,239,241,251,257,263,269,271,277,
    281,283,293,307,311,313,317,331,337,347,
    349,353,359,367,373,379,383,389,397,401,
    409,419,421,431,433,439,443,449,457,461,
    463,467,479,487,491,499,503,509,521,523,
    541,547,557,563,569,571,577,587,593,599,
    601,607,613,617,619,631,641,643,647,653,
    659,661,673,677,683,691,701,709,719,727,
    733,739,743,751,757,761,769,773,787,797,
    809,811,821,823,827,829,839,853,857,859,
    863,877,881,883,887,907,911,919,929,937,
    941,947,953,967,971,977,983,991,997
    };

Value Primes::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    if(!args[0].isNumber())
        return Value(Error(ErrorId::EXPECTED_NUMERIC_VALUE, args[0].getNumber().range));
    long long number = (long long) args[0].getNumber().to_double();
    auto numberList = factors(number);
    std::vector<Number> numbers;
    for (auto n : numberList)
        {
        auto lower = std::lower_bound(primes.begin(), primes.end(), n);
        if(lower != primes.end())
            if(*lower == n)
                numbers.push_back(Number((double)n,0, range));
        }
    return Value(numbers);
    }

Value Sum::execute(Scope& scope, std::vector<Value>& args, const Range& range)
    {
    double sum = 0;
    std::vector<Value> explodedList;
    auto& nums = explodeArgs(args, explodedList);
    for (auto& n : nums)
        {
        if(!n.isNumber())
            return Value(Error(ErrorId::EXPECTED_NUMERIC_VALUE, args[0].getNumber().range));
        sum += n.getNumber().to_double();
        }
    auto num = Number(sum, 0, range);
    return num;
    }