#include "pch.h"
#include "Number.h"
#include <sstream>
#include <bitset>
#include <iomanip>

Number& Number::operator++(int)
    {
    significand = to_double();
    significand++;
    exponent = 0;
    return *this;
    }

Number& Number::operator--(int)
    {
    significand = to_double();
    significand--;
    exponent = 0;
    return *this;
    }

double Number::to_double() const 
    { 
    if(isnan(significand))
        return significand;
    else
        return significand * std::pow(10, exponent); 
    }

Number Number::operator+(const Number& number2) const
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.significand += n2.significand;
    return n1;
    }

Number Number::operator-(const Number& number2) const
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.significand -= n2.significand;
    return n1;
    }

Number Number::operator*(const Number& n2) const
    {
    Number n = Number(significand*n2.significand, exponent+n2.exponent);
    n.unit = this->unit;
    return n;
    }

Number Number::operator/(const Number& n2) const
    {
    Number n = Number(significand/n2.significand, exponent-n2.exponent);
    n.unit = this->unit;
    return n;
    }

Number Number::convertToExponent(int e) const
    {
    Number result = *this;
    while (result.exponent < e)
        {
        result.exponent++;
        result.significand /=10;
        }
    while (result.exponent > e)
        {
        result.exponent--;
        result.significand *=10;
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
    
    if (unitDefs.exists(unit.id) == false)
        {
        return num; //invalid unit should already have been detected. Don't report the error again.
        }
    if (unitDefs.exists(to.id) == false)
        {
        num.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, to.range, to.id.c_str()));
        return num;
        }
    if (unitDefs.get(unit.id).property != unitDefs.get(to.id).property)
        {
        num.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, range));
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

std::string formatDouble(double d)
    {
    std::ostringstream numval;
    numval << std::fixed
        << std::setprecision(20)
        << d;
    std::string str = numval.str();
    if(str.find('.') != std::string::npos)
        {
        // Remove trailing zeroes
        str = str.substr(0, str.find_last_not_of('0')+1);
        // If the decimal point is now the last character, remove that as well
        if(str.find('.') == str.size()-1)
            {
            str = str.substr(0, str.size()-1);
            }
        }
    return str;
    }

std::string Number::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";


    std::ostringstream numval;
    numval << formatDouble(to_double());
    sstr << "\"value\":\"" << numval.str() << "\"";

    numval = std::ostringstream();
    numval << formatDouble(significand);
    sstr << ",\"significand\":\"" << numval.str() << "\"";
    sstr << ",\"exponent\":" << exponent;

    sstr << ",\"unit\":\"" << unit << "\"";

    std::string format = "DEC";
    if(numFormat == NumFormat::BIN)
        format = "BIN";
    else if(numFormat == NumFormat::HEX)
        format = "HEX";
    sstr << ",\"format\":\"" << format << "\"";

    std::string formatted;
    if (numFormat == NumFormat::BIN)
        {
        formatted = std::bitset<64>((long)to_double()).to_string();
        formatted.erase(0, formatted.find_first_not_of('0'));
        formatted = "0b" + formatted;
        }
    else if (numFormat == NumFormat::HEX)
        {
        std::ostringstream oss;
        oss << std::hex << (long)to_double();
        formatted = "0x"+oss.str();
        }

    sstr << ",\"formatted\":\""
        << formatted
        << "\"";

    sstr << "}";
    return sstr.str();
    }