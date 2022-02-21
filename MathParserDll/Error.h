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
    FUNC_NO_OPEN_PAR,
    W_DIV_IMPL_MULT,
    W_POW_IMPL_MULT,
    UNIT_NOT_DEF,
    W_ASSUMING_UNIT,
    VAR_EXPECTED,
    UNIT_PROP_DIFF,
    };


struct ErrorDef
    {
    ErrorId id = ErrorId::NONE;
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
        unsigned int line = -1;
        unsigned int pos = -1;

        Error() {}
        Error(ErrorId id, unsigned int line, unsigned int pos, va_list args);
        Error(ErrorId id, unsigned int line, unsigned int pos, ...);
        const std::string to_json();
    private:
        void buildErrorString(va_list args = nullptr);
    };

