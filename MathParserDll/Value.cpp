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
    sstr << ", \"unit\" : \"" << number.unit << "\""; //TODO: move unit under number.

    std::string format = "DEC";
    if(number.numFormat == NumFormat::BIN) //TODO: move numFormat under number.
        format = "BIN";
    else if(number.numFormat == NumFormat::HEX)
        format = "HEX";
    sstr << ", \"format\" : \"" << format << "\"";

    std::string formatted;
    if (number.numFormat == NumFormat::BIN)
        {
        formatted = std::bitset<64>((long)number.to_double()).to_string();
        formatted.erase(0, formatted.find_first_not_of('0'));
        formatted = "0b" + formatted;
        }
    else if (number.numFormat == NumFormat::HEX)
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
