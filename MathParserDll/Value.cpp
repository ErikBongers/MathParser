#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>
#include <bitset>
#include "Resolver.h"

Value::Value(Number n)
    : type(ValueType::NUMBER), number(n)
    {}

Value::Value(const Error& error)
    {
    errors.push_back(error);
    }

Value::Value(const std::vector<Error>& errors)
    {
    this->errors.insert(this->errors.begin(), errors.begin(), errors.end());
    }

Value::Value(Token id, Number n)
    : id(id), number(n), type(ValueType::NUMBER)
    {
    }

Value::Value(Date date)
    : type(ValueType::TIMEPOINT)
    {
    //TODO
    }


std::string Value::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";
    if (id.type != TokenType::NULLPTR)
        sstr << "\"id\" : \"" << id.stringValue << "\"";
    else
        sstr << "\"id\" : \"#result#\"";
    sstr << ", \"number\" : ";
    sstr << number.to_json();

    sstr << ", \"errors\" : [";
    std::string comma = "";
    for (auto& error : errors)
        {
        sstr << comma << error.to_json();
        comma = ",";
        }
    for (auto& error : number.errors)
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
