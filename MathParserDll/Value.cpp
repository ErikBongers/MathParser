#include "pch.h"
#include "Value.h"

Value::Value(ErrorId errorId, ...)
    {
    va_list args;
    va_start(args, errorId);
    errors.push_back(Error(errorId, args));
    va_end(args);
    }

Value::Value(Error& error)
    {
    errors.push_back(error);
    }

Value::Value(double d, Token u)
    {
    this->number = d;
    this->unit = u;
    }

Value::Value(Token id, double d, Token u)
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
    if (unit.type == TokenType::RAD)
        str << Value(number / M_PI * 180, Token(TokenType::DEG, "deg")).to_string();
    else
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
    if (unit.type == TokenType::NULLPTR)
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator-(const Value& v)
    {
    number -= v.number;
    if (unit.type == TokenType::NULLPTR)
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator*(const Value& v)
    {
    number *= v.number;
    if (unit.type == TokenType::NULLPTR)
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator/(const Value& v)
    {
    number /= v.number;
    if (unit.type == TokenType::NULLPTR)
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

Value& Value::operator^(const Value& v)
    {
    number = std::pow(number,v.number);
    if (unit.type == TokenType::NULLPTR)
        unit = v.unit;
    errors.insert(errors.end(), v.errors.begin(), v.errors.end());
    return *this;
    }

