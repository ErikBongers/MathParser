#pragma once
#include "pch.h"

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
    CONST_REDEF,
    W_VAR_IS_UNIT,
    W_VAR_IS_FUNCTION,
    };


struct ErrorDef
    {
    ErrorId id = ErrorId::NONE;
    const char* name = nullptr;
    const char* message = nullptr;
    bool isWarning() { return name[0] == 'W'; }
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
        Error(ErrorId id, unsigned int line, unsigned int pos);
        Error(ErrorId id, unsigned int line, unsigned int pos, const std::string& arg1);
        const std::string to_json();
        bool isWarning();
    };

bool hasRealErrors(std::vector<Error>& errors);

