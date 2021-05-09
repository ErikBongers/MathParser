#pragma once
#include "pch.h"
#include <cstdarg>
#include <map>

enum class ErrorId
    {
    NONE = 0,
    UNKNOWN_EXPR,
    VAR_NOT_DEF,
    FUNC_NOT_DEF,
    FUNC_ARG_MIS,
    W_DIV_IMPL_MULT,
    W_POW_IMPL_MULT,
    };


struct ErrorDef
    {
    ErrorId id;
    std::string name;
    std::string message;
    };

class ErrorDefs
    {
    public:
        static std::map<ErrorId, ErrorDef> errorDefs; 
        
        static const ErrorDef& get(ErrorId id) { return errorDefs[id]; }
    };


class Error
    {
    public:
        ErrorId id = ErrorId::NONE;
        std::string errorMsg;
        int line = -1;
        int pos = -1;

        Error() {}
        Error(ErrorId id, va_list args=nullptr);
        const std::string to_json();
    };

