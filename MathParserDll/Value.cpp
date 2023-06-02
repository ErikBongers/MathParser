#include "pch.hpp"
#include "Value.h"
#include "json.h"
#include <sstream>
#include <bitset>

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

Value::Value(List list)
    : type(ValueType::LIST)
    {
    data = list;
    }

std::string getText(const Range& range, const char* stream)
    {
    return std::string(&stream[range.start.cursorPos], &stream[range.end.cursorPos]);
    }

void Value::to_json(std::ostringstream& sstr, const char* stream) const
    {
    sstr << "{";
    sstr << "\"line\":" << this->stmtRange.start.line << "";
    if (id.type != TokenType::NULLPTR)
        sstr << ",\"id\":\"" << getText(id.range, stream) << "\"";
    else
        sstr << ",\"id\":\"_\"";
    sstr << ",\"type\":\"" << to_string(type) << "\"";
    const std::vector<Error>* pErrors = nullptr;
    if(type == ValueType::NUMBER)
        {
        sstr << ",\"number\":";
        std::get<Number>(data).to_json(sstr);
        pErrors = &std::get<Number>(data).errors;
        }
    else if (type == ValueType::TIMEPOINT)
        {
        sstr << ",\"date\":";
        std::get<Date>(data).to_json(sstr);
        pErrors = &std::get<Date>(data).errors;
        }
    else if (type == ValueType::DURATION)
        {
        sstr << ",\"duration\":";
        std::get<Duration>(data).to_json(sstr);
        }
    else if (type == ValueType::LIST)
        {
        sstr << ",\"values\":";
        std::get<List>(data).to_json(sstr);
        }

    sstr << ",\"errors\":[";
    std::string comma = "";
    for (auto& error : errors)
        {
        sstr << comma;
        error.to_json(sstr);
        comma = ",";
        }
    if(pErrors != nullptr)
        {
        for (auto& error : *pErrors)
            {
            sstr << comma;
            error.to_json(sstr);
            comma = ",";
            }
        }
    sstr << "]";
    sstr << ",\"text\":\""
        << escape_json(text)
        << "\"";
    sstr << ",\"comment\":";
    sstr <<  "\"" << escape_json(make_one_line(getText(comment_line, stream))) << "\"";
    sstr << ",\"onlyComment\": " << (onlyComment?"true":"false");
    sstr << ",\"mute\":" << (mute?"true":"false");
    sstr <<  ",\"range\":";
    stmtRange.to_json(sstr);
    sstr << "}";
    }
