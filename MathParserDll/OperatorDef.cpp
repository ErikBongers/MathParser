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

    if(this->type2 < id2.type2)
        return true;
    return false;
    }

std::string OperatorId::to_string()
    {
    return std::string("op ") + (char)type1 + (char)op + (char)type2;
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
    Add(new OpDateMinDate(*this));
    }

Number doTerm(UnitDefs& unitDefs, const Number& v1, bool adding, const Number& v, unsigned int line, unsigned int pos)
    {
    Number result = v1;
    
    //if both values have units: convert them to SI before operation.
    if (!v1.unit.isClear() && !v.unit.isClear())
        {
        if(unitDefs.exists(v1.unit.id) && unitDefs.exists(v.unit.id))
            if (unitDefs.get(v1.unit.id).property != unitDefs.get(v.unit.id).property)
                {
                result.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, v1.line, v1.pos));
                return result;
                }
        double d1 = v1.toSI(unitDefs);
        double d2 = v.toSI(unitDefs);
        result = Number(adding ? (d1 + d2) : (d1 - d2), 0);
        if(unitDefs.exists(v1.unit.id))
            result = Number(unitDefs.get(v1.unit.id).fromSI(result.to_double()), v1.unit.id, v1.numFormat, line, pos); //TODO: try to keep exponent.
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
            result.errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, v1.line, v1.pos));
            }
        }
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    result.line = line;
    result.pos = pos;
    return result;
    }

Value OpNumPlusNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(doTerm(defs.unitDefs, args[0].getNumber(), true, args[1].getNumber(), line, pos)));
    }

Value OpNumMinNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    return Value(Number(doTerm(defs.unitDefs, args[0].getNumber(), false, args[1].getNumber(), line, pos)));
    }

Value OpNumMultNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
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

Value OpNumDivNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
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

Value OpNumPowNum::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Value result = args[0];
    result.constant = false;
    result.setNumber(Number(std::pow(args[0].getNumber().to_double(), args[1].getNumber().to_double()), 0));
    if (!args[0].getNumber().unit.isClear())
        result.getNumber().unit = args[1].getNumber().unit;
    result.errors.insert(result.errors.end(), args[1].errors.begin(), args[1].errors.end());
    return result;
    }

Value OpDateMinDate::execute(std::vector<Value>& args, unsigned int line, unsigned int pos)
    {
    Date d1 = args[0].getDate();
    Date d2 = args[1].getDate();
    auto dd = d1 - d2;
    dd.normalize();
    return dd;
    }
