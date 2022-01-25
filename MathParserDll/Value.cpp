#include "pch.h"
#include "Value.h"

Value::Value(ErrorId errorId, ...)
    {
    va_list args;
    va_start(args, errorId);
    errors.push_back(Error(errorId, args));
    va_end(args);
    }

Value::Value(const Error& error)
    {
    errors.push_back(error);
    }

Value::Value(const std::vector<Error>& errors)
    {
    this->errors.insert(this->errors.begin(), errors.begin(), errors.end());
    }

Value::Value(double d, const Unit u)
    {
    this->number = d;
    this->unit = u;
    }

Value::Value(Token id, double d, const Unit u)
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

Value& Value::operator+(const Value& v)
    {
    number += v.number;
    if (!unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator-(const Value& v)
    {
    number -= v.number;
    if (!unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator*(const Value& v)
    {
    number *= v.number;
    if (!unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator/(const Value& v)
    {
    number /= v.number;
    if (!unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator^(const Value& v)
    {
    number = std::pow(number,v.number);
    if (!unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value Value::convertToUnit(const Unit& to)
    {
    Value value = *this;
    double fFrom = 1;
    double fTo = 1;

    if (UnitDef::defs.count(this->unit.id))
        fFrom = UnitDef::defs[this->unit.id].toSI;
    else
        value.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, this->unit.id.c_str()));
    
    if (UnitDef::defs.count(to.id))
        fTo = UnitDef::defs[to.id].toSI;
    else
        value.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, to.id.c_str()));
    
    value.number = this->number * fFrom/ fTo;
    value.unit = to;
    return value;
    }
