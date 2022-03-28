#include "pch.h"
#include "Value.h"
#include "json.h"
#include <sstream>
#include <bitset>
#include "Resolver.h"

Value::Value(Number n)
    : type(ValueType::NUMBER)
    {
    data = n;
    }

Value::Value(const Error& error)
    {
    errors.push_back(error);
    }

Value::Value(const std::vector<Error>& errors)
    {
    this->errors.insert(this->errors.begin(), errors.begin(), errors.end());
    }

Value::Value(Token id, Number n)
    : id(id), type(ValueType::NUMBER)
    {
    data = n;
    }

Value::Value(Date date)
    : type(ValueType::TIMEPOINT)
    {
    data = date;
    }

Value::Value(Duration duration)
    : type(ValueType::DURATION)
    {
    data = duration;
    }

std::string Value::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";
    if (id.type != TokenType::NULLPTR)
        sstr << "\"id\" : \"" << id.stringValue << "\"";
    else
        sstr << "\"id\" : \"#result#\"";
    sstr << ", \"type\" : \"" << to_string(type) << "\"";
    std::vector<Error>* pErrors = nullptr;
    if(type == ValueType::NUMBER)
        {
        sstr << ", \"number\" : ";
        sstr << std::get<Number>(data).to_json();
        pErrors = &std::get<Number>(data).errors;
        }
    else if (type == ValueType::TIMEPOINT)
        {
        sstr << ", \"date\": ";
        sstr << std::get<Date>(data).to_json();
        pErrors = &std::get<Date>(data).errors;
        }
    else if (type == ValueType::DURATION)
        {
        sstr << ", \"duration\": ";
        sstr << std::get<Duration>(data).to_json();
        }

    sstr << ", \"errors\" : [";
    std::string comma = "";
    for (auto& error : errors)
        {
        sstr << comma << error.to_json();
        comma = ",";
        }
    if(pErrors != nullptr)
        {
        for (auto& error : *pErrors)
            {
            sstr << comma << error.to_json();
            comma = ",";
            }
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
