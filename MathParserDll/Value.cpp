#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>

Value::Value(ErrorId errorId, unsigned int line, unsigned int pos ...)
    : line(line), pos(pos)
    {
    va_list args;
    va_start(args, pos);
    errors.push_back(Error(errorId, line, pos, args));
    va_end(args);
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
    sstr << ", \"comments\" : [";
    comma = "";
    for (auto& comm : comment_lines)
        {
        sstr << comma << "\"" << escape_json(make_one_line(comm)) << "\"";
        comma = ",";
        }
    sstr << "]";
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

Value& Value::operator+(const Value& v)
    {
    return doTerm(true, v);
    }

Value& Value::operator-(const Value& v)
    {
    return doTerm(false, v);
    }

Value& Value::doTerm(bool adding, const Value& v)
    {
    //if both values have units: convert them to SI before operation.
    if (!unit.id.empty() && !v.unit.id.empty())
        {
        if (UnitDef::defs[unit.id].property != UnitDef::defs[v.unit.id].property)
            {
            errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
            return *this;
            }
        double d1 = this->toSI();
        double d2 = v.toSI();
        number = adding ? (d1 + d2) : (d1 - d2);
        number = UnitDef::defs[this->unit.id].fromSI(number);
        }
    //if both values have no units, just do operation.
    else if (unit.id.empty() && v.unit.id.empty())
        {
        number = adding ? (number + v.number) : (number - v.number);
        }
    else //a value with a unit and one without it: assuming both same unit
        {
        number = adding ? (number + v.number) : (number - v.number);
        if (this->unit.id.empty())
            this->unit = v.unit;
        errors.push_back(Error(ErrorId::W_ASSUMING_UNIT, line, pos));
        }
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator*(const Value& v)
    {
    //TODO: if both units set: unit changes to unit*unit!
    number *= v.number;
    if (unit.id.empty())
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator/(const Value& v)
    {
    //TODO: if both units set: unit changes to unit/unit!
    number /= v.number;
    if (unit.id.empty())
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

    if (UnitDef::defs.count(this->unit.id) == 0)
        {
        value.unit = to;
        return value;
        }
    
    if (UnitDef::defs.count(to.id) == 0)
        {
        value.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, line, pos, to.id.c_str()));
        return value;
        }
    if (UnitDef::defs[unit.id].property != UnitDef::defs[to.id].property)
        {
        value.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
        return value;
        }
    value.number = UnitDef::defs[this->unit.id].toSI(this->number); //from -> SI
    value.number = UnitDef::defs[to.id].fromSI(value.number);  //SI -> to
    value.unit = to;
    return value;
    }
