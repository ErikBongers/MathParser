#include "pch.h"
#include "Error.h"
#include "json.h"
#include <sstream>

Error::Error(ErrorId id, unsigned int line, unsigned int pos, va_list args)
    : line(line), pos(pos)
    {
    this->id = id;
    buildErrorString(args);
    }

void Error::buildErrorString(va_list args)
    {
    char buffer[200];
    vsnprintf(buffer, 200, ErrorDefs::get(id).message.c_str(), args);
    this->errorMsg = buffer;
    }


Error::Error(ErrorId id, unsigned int line, unsigned int pos, ...)
    : line(line), pos(pos)
    {
    this->id = id;

    va_list args;
    va_start(args, pos);
    buildErrorString(args);
    va_end(args);
    }

const std::string Error::to_json()
    {
    auto it = ErrorDefs::errorDefs.find(id);
    std::ostringstream sstr;
    sstr << "{";
    if(it == ErrorDefs::errorDefs.end())
        { 
        sstr << "\"id\" : \"??\"";
        }
    else
        {
        auto& ed = it->second;
        sstr << "\"id\" : \"" + ed.name + "\", \"message\" : \"" + escape_json(errorMsg) + "\"";
        }
    sstr << ", \"line\": " << line << ", \"pos\": " << pos;
    sstr << "}";
    return sstr.str();
    }

std::map<ErrorId, ErrorDef> ErrorDefs::errorDefs =
    {
    {ErrorId::UNKNOWN_EXPR, {ErrorId::UNKNOWN_EXPR, "UNKNOWN_EXPR", "Unknown expression."}},
    {ErrorId::VAR_NOT_DEF, {ErrorId::VAR_NOT_DEF, "VAR_NOT_DEF", "variable \"%s\" is not defined."}},
    {ErrorId::FUNC_NOT_DEF, {ErrorId::FUNC_NOT_DEF, "FUNC_NOT_DEF", "function \"%s\" is not defined."}},
    {ErrorId::FUNC_ARG_MIS, {ErrorId::FUNC_ARG_MIS, "FUNC_ARG_MIS", "argument(s) for function \"%s\" don't match."}},
    {ErrorId::FUNC_NO_OPEN_PAR, {ErrorId::FUNC_NO_OPEN_PAR, "FUNC_NO_OPEN_PAR", "Missing '(' for function \"%s\"."}},
    {ErrorId::W_DIV_IMPL_MULT, {ErrorId::W_DIV_IMPL_MULT, "W_DIV_IMPL_MULT", "Warning: ambiguous expression: division followed by implicit multiplication."}},
    {ErrorId::W_POW_IMPL_MULT, {ErrorId::W_POW_IMPL_MULT, "W_POW_IMPL_MULT", "Warning: ambiguous expression: exponential combined with implicit multiplication."}},
    {ErrorId::UNIT_NOT_DEF, {ErrorId::UNIT_NOT_DEF, "UNIT_NOT_DEF", "unit \"%s\" is not defined."}},
    {ErrorId::W_ASSUMING_UNIT, {ErrorId::W_ASSUMING_UNIT, "W_ASSUMING_UNIT", "Warning: not all values have a unit. Assuming same unit as the other values."}},
    {ErrorId::VAR_EXPECTED, {ErrorId::VAR_EXPECTED, "VAR_EXPECTED", "The increment (++) or decrement (--) operator can only be used on a variable."}},
    {ErrorId::UNIT_PROP_DIFF, {ErrorId::UNIT_PROP_DIFF, "UNIT_PROP_DIFF", "The units are not for the same property (lenght, temperature,...)."}},
    };
