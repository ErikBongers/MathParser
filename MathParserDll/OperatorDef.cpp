#include "pch.hpp"
#include "OperatorDef.h"
#include "Value.h"
#include "Resolver.h"
#include "Globals.h"
#include "Scope.h"

bool OperatorId::operator<(OperatorId const& id2) const
    {
    if(this->type1 < id2.type1)
        return true;
    if(this->type1 > id2.type1)
        return false;

    if(this->op < id2.op)
        return true;
    if(this->op > id2.op)
        return false;

    if(this->type2 < id2.type2)
        return true;
    return false;
    }

std::string OperatorId::to_string()
    {
    return std::string("op ") + (char)type1 + (char)op + (char)type2;
    }

Number doTerm(UnitsView& units, const Number& v1, bool adding, const Number& v, const Range& range)
    {
    Number result = v1;
    
    //if both values have units: convert them to SI before operation.
    if (!v1.unit.isClear() && !v.unit.isClear())
        {
        if(units.exists(v1.unit.id) && units.exists(v.unit.id))
            if (units.get(v1.unit.id).property != units.get(v.unit.id).property)
                {
                result.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, range));
                return result;
                }
        Number d1 = v1.toSI(units);
        Number d2 = v.toSI(units);
        result = adding ? (d1 + d2) : (d1 - d2);
        if(units.exists(v1.unit.id))
            result = Number(units.get(v1.unit.id).fromSI(result.significand), result.exponent, v1.unit, v1.numFormat, range);
        }
    //if a unit is missing, just do operation.
    else 
        {
        result = adding ? (v1 + v) : (v1 - v);
        //if one unit is set, use it but give a warning
        if (!v1.unit.isClear() || !v.unit.isClear())
            {
            if (v1.unit.isClear())
                result.unit = v.unit;
            result.errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, range));
            }
        }
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    result.range = range;
    return result;
    }

Value OpNumPlusNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(doTerm(globals.unitsView, args[0].getNumber(), true, args[1].getNumber(), range)));
    }

Value OpNumMinNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    return Value(Number(doTerm(globals.unitsView, args[0].getNumber(), false, args[1].getNumber(), range)));
    }

Value OpNumMultNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Value result = args[0];
    result.constant = false;
    //TODO: if both units set: unit changes to unit*unit!
    result.setNumber(result.getNumber() * args[1].getNumber());
    if (args[0].getNumber().unit.isClear())
        result.getNumber().unit = args[1].getNumber().unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpNumDivNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Value result = args[0];
    result.constant = false;
    //TODO: if both units set: unit changes to unit/unit!
    result.setNumber(result.getNumber() / args[1].getNumber());
    if (args[0].getNumber().unit.isClear())
        result.getNumber().unit = args[1].getNumber().unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpNumPowNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Value result = args[0];
    result.constant = false;
    result.setNumber(Number(std::pow(args[0].getNumber().to_double(), args[1].getNumber().to_double()), 0, result.getNumber().range));
    if (!args[0].getNumber().unit.isClear())
        result.getNumber().unit = args[1].getNumber().unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpDateMinDate(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Date d1 = args[0].getDate();
    Date d2 = args[1].getDate();
    auto dd = d1 - d2;
    dd.normalize();
    return dd;
    }

Value OpDatePlusDur(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Date date = args[0].getDate();
    Duration dur = args[1].getDuration();
    auto dd = date + dur;
    return dd;
    }

Value OpDatePlusNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Date date = args[0].getDate();
    Number num = args[1].getNumber();
    auto dd = date + num;
    return dd;
    }

Value OpDurPlusNum(Globals& globals, std::vector<Value>& args, const Range& range)
    {
    Duration dur = args[0].getDuration();
    Number num = args[1].getNumber();
    auto dd = dur + num;
    return dd;
    }

void OperatorDefs::init()
    {
    Add(OperatorId(ValueType::NUMBER, OperatorType::PLUS, ValueType::NUMBER, ValueType::NUMBER), OpNumPlusNum);
    Add(OperatorId(ValueType::NUMBER, OperatorType::MIN, ValueType::NUMBER, ValueType::NUMBER), OpNumMinNum);
    Add(OperatorId(ValueType::NUMBER, OperatorType::MULT, ValueType::NUMBER, ValueType::NUMBER), OpNumMultNum);
    Add(OperatorId(ValueType::NUMBER, OperatorType::DIV, ValueType::NUMBER, ValueType::NUMBER), OpNumDivNum);
    Add(OperatorId(ValueType::NUMBER, OperatorType::POW, ValueType::NUMBER, ValueType::NUMBER), OpNumPowNum);
    Add(OperatorId(ValueType::TIMEPOINT, OperatorType::MIN, ValueType::TIMEPOINT, ValueType::DURATION), OpDateMinDate);
    Add(OperatorId(ValueType::TIMEPOINT, OperatorType::PLUS, ValueType::DURATION, ValueType::TIMEPOINT), OpDatePlusDur);
    Add(OperatorId(ValueType::TIMEPOINT, OperatorType::PLUS, ValueType::NUMBER, ValueType::TIMEPOINT), OpDatePlusNum);
    Add(OperatorId(ValueType::DURATION, OperatorType::PLUS, ValueType::NUMBER, ValueType::DURATION), OpDurPlusNum);
    }















