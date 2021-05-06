#include "pch.h"
#include "Error.h"

Error::Error(ErrorId id, va_list args)
    {
    this->id = id;
    char buffer[200];
    this->errorMsg = snprintf(buffer, 200, ErrorDefs::getMsg(id).c_str(), args);
    }

std::map<ErrorId, std::string> ErrorDefs::errorDefs =
    {
    {ErrorId::UNKNOWN_EXPR,  "Unknown expression."},
    {ErrorId::VAR_NOT_DEF,  "variable \"%s\" is not defined."},
    {ErrorId::FUNC_NOT_DEF,  "function \"%s\" is not defined."},
    {ErrorId::FUNC_ARG_MIS,  "argument(s) for function \"%s\" missing."},
    {ErrorId::W_DIV_IMPL_MULT,  "Warning: ambiguous expression: division followed by implicit multiplication."},
    {ErrorId::W_POW_IMPL_MULT,  "Warning: ambiguous expression: exponential combined with implicit multiplication."},
    };
