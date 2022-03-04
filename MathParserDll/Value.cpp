#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>

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

Value::Value(double d, const Unit u, unsigned int line, unsigned int pos)
    : line(line), pos(pos)
    {
    this->number = d;
    this->unit = u;
    }

Value::Value(Token id, double d, const Unit u, unsigned int line, unsigned int pos)
    : line(line), pos(pos)
    {
    this->id = id;
    this->number = d;
    this->unit = u;
    }

std::string Value::to_string()
    {
    std::ostringstream strs;
    strs << std::fixed
        << std::setprecision(5)
        << number
        << unit;
    return strs.str();
    }

std::string Value::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";
    if (id.type != TokenType::NULLPTR)
        sstr << "\"id\" : \"" << id << "\"";
    else
        sstr << "\"id\" : \"#result#\"";
    if (isnan(number))
        sstr << ", \"value\" : \"NaN\"";
    else
        {
        sstr << ", \"value\" : \"" 
            << std::fixed
            << std::setprecision(20)
            << number << "\"";
        sstr << ", \"unit\" : \"" << unit << "\"";
        }
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
        result.number = adding ? (d1 + d2) : (d1 - d2);
        if(UnitDef::exists(this->unit.id.stringValue))
            result.number = UnitDef::get(this->unit.id.stringValue).fromSI(result.number);
        }
    //if both values have no units, just do operation.
    else if (unit.isClear() && v.unit.isClear())
        {
        result.number = adding ? (number + v.number) : (number - v.number);
        }
    else //a value with a unit and one without it: assuming both same unit
        {
        result.number = adding ? (number + v.number) : (number - v.number);
        if (this->unit.isClear())
            result.unit = v.unit;
        result.errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, line, pos));
        }
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::operator*(const Value& v)
    {
    Value result = *this;
    result.constant = false;
    //TODO: if both units set: unit changes to unit*unit!
    result.number *= v.number;
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
    result.number /= v.number;
    if (unit.isClear())
        result.unit = v.unit;
    result.errors.insert(result.errors.end(), v.errors.begin(), v.errors.end());
    return result;
    }

Value Value::operator^(const Value& v)
    {
    Value result = *this;
    result.constant = false;
    result.number = std::pow(number,v.number);
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

    if (UnitDef::exists(this->unit.id.stringValue) == false)
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
    value.number = UnitDef::get(this->unit.id.stringValue).toSI(this->number); //from -> SI
    value.number = UnitDef::get(to.id.stringValue).fromSI(value.number);  //SI -> to
    value.unit = to;
    return value;
    }

inline double Value::toSI() const 
    { 
    if(UnitDef::exists(unit.id.stringValue))
        return UnitDef::get(unit.id.stringValue).toSI(number);
    else
        return number;
    }

inline double Value::fromSI() const 
    { 
    if(UnitDef::exists(unit.id.stringValue))
        return UnitDef::get(unit.id.stringValue).fromSI(number); 
    else
        return number;
    }
