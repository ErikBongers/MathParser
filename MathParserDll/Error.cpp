#include "pch.h"
#include "Error.h"
#include "json.h"

Error::Error(ErrorId id, va_list args)
    {
    this->id = id;
    char buffer[200];
    vsnprintf(buffer, 200, ErrorDefs::get(id).message.c_str(), args);
    this->errorMsg = buffer;
    }

const std::string Error::to_json()
    {
    auto it = ErrorDefs::errorDefs.find(id);
    std::string str;
    if(it == ErrorDefs::errorDefs.end())
        { 
        str = "{ \"id\" : \"??\" }";
        }
    else
        {
        auto& ed = it->second;
        str = "{ \"id\" : \"" + ed.name + "\", \"message\" : \"" + escape_json(errorMsg) + "\"}";
        }
    return str;
    }

std::map<ErrorId, ErrorDef> ErrorDefs::errorDefs =
    {
    {ErrorId::UNKNOWN_EXPR, {ErrorId::UNKNOWN_EXPR, "UNKNOWN_EXPR", "Unknown expression."}},
    {ErrorId::VAR_NOT_DEF, {ErrorId::VAR_NOT_DEF, "VAR_NOT_DEF", "variable \"%s\" is not defined."}},
    {ErrorId::FUNC_NOT_DEF, {ErrorId::FUNC_NOT_DEF, "FUNC_NOT_DEF", "function \"%s\" is not defined."}},
    {ErrorId::FUNC_ARG_MIS, {ErrorId::FUNC_ARG_MIS, "FUNC_ARG_MIS", "argument(s) for function \"%s\" missing."}},
    {ErrorId::W_DIV_IMPL_MULT, {ErrorId::W_DIV_IMPL_MULT, "W_DIV_IMPL_MULT", "Warning: ambiguous expression: division followed by implicit multiplication."}},
    {ErrorId::W_POW_IMPL_MULT, {ErrorId::W_POW_IMPL_MULT, "W_POW_IMPL_MULT", "Warning: ambiguous expression: exponential combined with implicit multiplication."}}
    };
