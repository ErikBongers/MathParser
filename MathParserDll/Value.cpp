#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>
#include <bitset>
#include "Resolver.h"

Value::Value(Number n, unsigned int line, unsigned int pos)
    : line(line), pos(pos), type(ValueType::NUMBER), number(n)
    {}

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
    : line(line), pos(pos), number(n), unit(u), type(ValueType::NUMBER)
    {
    }

Value::Value(Token id, Number n, const Unit u, unsigned int line, unsigned int pos)
    : line(line), pos(pos), id(id), number(n), unit(u), type(ValueType::NUMBER)
    {
    }

Value::Value(Date date, unsigned int line, unsigned int pos)
    : line(0), pos(0), type(ValueType::TIMEPOINT)
    {
    //TODO
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

Value Value::convertToUnit(const Unit& to, UnitDefs& unitDefs)
    {
    Value value = *this;
    double fFrom = 1;
    double fTo = 1;

    if (unit.isClear())
        {
        value.unit = to;
        return value;
        }
    
    if (unitDefs.exists(to.id.stringValue) == false)
        {
        value.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, to.id.line, to.id.pos, to.id.stringValue.c_str()));
        return value;
        }
    if (unitDefs.get(unit.id.stringValue).property != unitDefs.get(to.id.stringValue).property)
        {
        value.errors.push_back(Error(ErrorId::UNIT_PROP_DIFF, line, pos));
        return value;
        }
    value.number = Number(unitDefs.get(this->unit.id.stringValue).toSI(this->number.to_double()), 0); //from -> SI
    value.number = Number(unitDefs.get(to.id.stringValue).fromSI(value.number.to_double()), 0);  //SI -> to
    value.unit = to;
    return value;
    }

double Value::toSI(UnitDefs& unitDefs) const 
    { 
    if(unitDefs.exists(unit.id.stringValue))
        return unitDefs.get(unit.id.stringValue).toSI(number.to_double());
    else
        return number.to_double();
    }

double Value::fromSI(UnitDefs& unitDefs) const 
    { 
    if(unitDefs.exists(unit.id.stringValue))
        return unitDefs.get(unit.id.stringValue).fromSI(number.to_double()); 
    else
        return number.to_double();
    }

