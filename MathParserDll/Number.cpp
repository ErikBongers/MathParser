#include "pch.h"
#include "Number.h"

Number& Number::operator++(int)
    {
    number = to_double();
    number++;
    exponent = 0;
    return *this;
    }

Number& Number::operator--(int)
    {
    number = to_double();
    number--;
    exponent = 0;
    return *this;
    }

Number Number::operator+(const Number& number2) const
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.number += n2.number;
    return n1;
    }

Number Number::operator-(const Number& number2) const
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.number -= n2.number;
    return n1;
    }

Number Number::operator*(const Number& n2) const
    {
    Number n = Number(number*n2.number, exponent+n2.exponent);
    n.unit = this->unit;
    return n;
    }

Number Number::operator/(const Number& n2) const
    {
    Number n = Number(number/n2.number, exponent-n2.exponent);
    n.unit = this->unit;
    return n;
    }

Number Number::convertToExponent(int e) const
    {
    Number result = *this;
    while (result.exponent < e)
        {
        result.exponent++;
        result.number /=10;
        }
    while (result.exponent > e)
        {
        result.exponent--;
        result.number *=10;
        }
    return result;
    }

Number Number::convertToUnit(const Unit& to, UnitDefs& unitDefs)
    {
    Number num = *this;
    double fFrom = 1;
    double fTo = 1;

    if (unit.isClear())
        {
        num.unit = to;
        return num;
        }
    
    if (unitDefs.exists(to.id) == false)
        {
        num.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, to.line, to.pos, to.id.c_str()));
        return num;
        }
    if (unitDefs.get(unit.id).property != unitDefs.get(to.id).property)
        {
        num.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
        return num;
        }
    num = Number(unitDefs.get(this->unit.id).toSI(this->to_double()), 0); //from -> SI
    num = Number(unitDefs.get(to.id).fromSI(num.to_double()), 0);  //SI -> to
    num.unit = to;
    return num;
    }

double Number::toSI(UnitDefs& unitDefs) const 
    { 
    if(unitDefs.exists(unit.id))
        return unitDefs.get(unit.id).toSI(to_double());
    else
        return to_double();
    }

double Number::fromSI(UnitDefs& unitDefs) const 
    { 
    if(unitDefs.exists(unit.id))
        return unitDefs.get(unit.id).fromSI(to_double()); 
    else
        return to_double();
    }

