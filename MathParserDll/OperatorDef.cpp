#include "pch.h"
#include "OperatorDef.h"
#include "Value.h"
#include "Resolver.h"

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

    if(this->type2 < this->type2)
        return true;
    return false;
    }

std::string OperatorId::to_string()
    {
    return "op " + (char)type1 + (char)op + (char)type2;
    }

Value OperatorDef::call(std::vector<Value>& args, unsigned int line, unsigned int pos)
    { 
    //TODO: check args

    return execute(args, line, pos); 
    }


void OperatorDefs::init()
    {
    Add(new OpNumPlusNum(*this));
    Add(new OpNumMinNum(*this));
    Add(new OpNumMultNum(*this));
    Add(new OpNumDivNum(*this));
    Add(new OpNumPowNum(*this));
    }

Value doTerm(UnitDefs& unitDefs, const Value& v1, bool adding, const Value& v)
    {
    Value result = v1;
    result.constant = false;
    //if both values have units: convert them to SI before operation.
    if (!v1.unit.isClear() && !v.unit.isClear())
        {
        if(unitDefs.exists(v1.unit.id.stringValue) && unitDefs.exists(v.unit.id.stringValue))
            if (unitDefs.get(v1.unit.id.stringValue).property != unitDefs.get(v.unit.id.stringValue).property)
                {
                result.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, v1.line, v1.pos));
                return result;
                }
        double d1 = v1.toSI();
        double d2 = v.toSI();
        result.number = Number(adding ? (d1 + d2) : (d1 - d2), 0);
        if(unitDefs.exists(v1.unit.id.stringValue))
            result.number = Number(unitDefs.get(v1.unit.id.stringValue).fromSI(result.number.to_double()), 0); //TODO: try to keep exponent.
        }
    //if a unit is missing, just do operation.
    else 
        {
        result.number = adding ? (v1.number + v.number) : (v1.number - v.number);
        //if one unit is set, use it but give a warning
        if (!v1.unit.isClear() || !v.unit.isClear())
            {
            if (v1.unit.isClear())
                result.unit = v.unit;
            result.errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, v1.line, v1.pos));
            }
        }
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value OpNumPlusNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return doTerm(defs.unitDefs, args[0], true, args[1]);
    }

Value OpNumMinNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return doTerm(defs.unitDefs, args[0], false, args[1]);
    }

Value OpNumMultNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value result = args[0];
    result.constant = false;
    //TODO: if both units set: unit changes to unit*unit!
    result.number = result.number * args[1].number;
    if (args[0].unit.isClear())
        result.unit = args[1].unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpNumDivNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value result = args[0];
    result.constant = false;
    //TODO: if both units set: unit changes to unit/unit!
    result.number = result.number / args[1].number;
    if (args[0].unit.isClear())
        result.unit = args[1].unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpNumPowNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value result = args[0];
    result.constant = false;
    result.number = Number(std::pow(args[0].number.to_double(), args[1].number.to_double()), 0);
    if (!args[0].unit.isClear())
        result.unit = args[1].unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

