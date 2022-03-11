#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>
#include <bitset>

Value::Value(ErrorId errorId, unsigned int line, unsigned int pos)
    : line(line), pos(pos)
    {
    errors.push_back(Error(errorId, line, pos));
    }

Value::Value(ErrorId errorId, unsigned int line, unsigned int pos, const std::string& arg1)
    : line(line), pos(pos)
    {
    errors.push_back(Error(errorId, line, pos, arg1));
    }

Value::Value(const Error& error)
    : line(error.line), pos(error.pos)
    {
    errors.push_back(error);
    }

Value::Value(const std::vector<Error>& errors)
    : line(0), pos(0)
    {
    this->errors.insert(this->errors.begin(), errors.begin(), errors.end());
    }

Value::Value(Number n, const Unit u, unsigned int line, unsigned int pos)
    : line(line), pos(pos), number(n), unit(u)
    {
    }

Value::Value(Token id, Number n, const Unit u, unsigned int line, unsigned int pos)
    : line(line), pos(pos), id(id), number(n), unit(u)
    {
    }

std::string Value::to_string() //TODO: unused?
    {
    std::ostringstream strs;
    strs << std::fixed
        << std::setprecision(5)
        << number.number
        << unit;
    return strs.str();
    }

std::string Value::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";
    if (id.type != TokenType::NULLPTR)
        sstr << "\"id\" : \"" << id.stringValue << "\"";
    else
        sstr << "\"id\" : \"#result#\"";
    std::ostringstream numval;
    if (isnan(number.number))
        numval << "NaN";
    else
        {
        numval << std::fixed
            << std::setprecision(20)
            << number.to_double();
        }
    sstr << ", \"value\" : \"" << numval.str() << "\"";
    numval = std::ostringstream();
    if (isnan(number.number))
        numval << "NaN";
    else
        {
        numval << std::fixed
            << std::setprecision(20)
            << number.number;
        }
    sstr << ", \"number\" : \"" << numval.str() << "\"";
    sstr << ", \"unit\" : \"" << unit << "\"";

    std::string format = "DEC";
    if(numFormat == NumFormat::BIN)
        format = "BIN";
    else if(numFormat == NumFormat::HEX)
        format = "HEX";
    sstr << ", \"format\" : \"" << format << "\"";

    std::string formatted;
    if (numFormat == NumFormat::BIN)
        {
        formatted = std::bitset<64>((long)number.to_double()).to_string();
        formatted.erase(0, formatted.find_first_not_of('0'));
        formatted = "0b" + formatted;
        }
    else if (numFormat == NumFormat::HEX)
        {
        std::ostringstream oss;
        oss << std::hex << (long)number.to_double();
        formatted = "0x"+oss.str();
        }

    sstr << ", \"formatted\" : \""
        << formatted
        << "\"";

    sstr << ", \"exponent\" : " << number.exponent;

    sstr << ", \"errors\" : [";
    std::string comma = "";
    for (auto& error : errors)
        {
        sstr << comma << error.to_json();
        comma = ",";
        }
    sstr << "]";
    sstr << ", \"text\" : \""
        << escape_json(text)
        << "\"";
    sstr << ", \"comment\" : ";
    sstr <<  "\"" << escape_json(make_one_line(comment_line)) << "\"";
    sstr << ", \"onlyComment\": " << (onlyComment?"true":"false");
    sstr << ", \"mute\": " << (mute?"true":"false");
    sstr << "}";
    return sstr.str();
    }

std::string Value::to_string(const std::string& format)
    {
    std::ostringstream str;
    str << to_string();
    if (errors.size() > 0)
        {
        for(auto& err: errors)
            str << "  <<< " + err.errorMsg;
        }
    return str.str();
    }

Value Value::operator+(const Value& v)
    {
    return doTerm(true, v);
    }

Value Value::operator-(const Value& v)
    {
    return doTerm(false, v);
    }

Value Value::doTerm(bool adding, const Value& v)
    {
    Value result = *this;
    result.constant = false;
    //if both values have units: convert them to SI before operation.
    if (!unit.isClear() && !v.unit.isClear())
        {
        if(UnitDef::exists(unit.id.stringValue) && UnitDef::exists(v.unit.id.stringValue))
            if (UnitDef::get(unit.id.stringValue).property != UnitDef::get(v.unit.id.stringValue).property)
                {
                result.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
                return result;
                }
        double d1 = this->toSI();
        double d2 = v.toSI();
        result.number = Number(adding ? (d1 + d2) : (d1 - d2), 0);
        if(UnitDef::exists(this->unit.id.stringValue))
            result.number = Number(UnitDef::get(this->unit.id.stringValue).fromSI(result.number.to_double()), 0); //TODO: try to keep exponent.
        }
    //if a unit is missing, just do operation.
    else 
        {
        result.number = adding ? (number + v.number) : (number - v.number);
        //if one unit is set, use it but give a warning
        if (!unit.isClear() || !v.unit.isClear())
            {
            if (this->unit.isClear())
                result.unit = v.unit;
            result.errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, line, pos));
            }
        }
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::operator*(const Value& v)
    {
    Value result = *this;
    result.constant = false;
    //TODO: if both units set: unit changes to unit*unit!
    result.number = result.number * v.number;
    if (unit.isClear())
        result.unit = v.unit;
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::operator/(const Value& v)
    {
    Value result = *this;
    result.constant = false;
    //TODO: if both units set: unit changes to unit/unit!
    result.number = result.number / v.number;
    if (unit.isClear())
        result.unit = v.unit;
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::operator^(const Value& v)
    {
    Value result = *this;
    result.constant = false;
    result.number = Number(std::pow(number.to_double(), v.number.to_double()), 0);
    if (!unit.isClear())
        result.unit = v.unit;
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::convertToUnit(const Unit& to)
    {
    Value value = *this;
    double fFrom = 1;
    double fTo = 1;

    if (unit.isClear())
        {
        value.unit = to;
        return value;
        }
    
    if (UnitDef::exists(to.id.stringValue) == false)
        {
        value.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, to.id.line, to.id.pos, to.id.stringValue.c_str()));
        return value;
        }
    if (UnitDef::get(unit.id.stringValue).property != UnitDef::get(to.id.stringValue).property)
        {
        value.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
        return value;
        }
    value.number = Number(UnitDef::get(this->unit.id.stringValue).toSI(this->number.to_double()), 0); //from -> SI
    value.number = Number(UnitDef::get(to.id.stringValue).fromSI(value.number.to_double()), 0);  //SI -> to
    value.unit = to;
    return value;
    }

inline double Value::toSI() const 
    { 
    if(UnitDef::exists(unit.id.stringValue))
        return UnitDef::get(unit.id.stringValue).toSI(number.to_double());
    else
        return number.to_double();
    }

inline double Value::fromSI() const 
    { 
    if(UnitDef::exists(unit.id.stringValue))
        return UnitDef::get(unit.id.stringValue).fromSI(number.to_double()); 
    else
        return number.to_double();
    }

