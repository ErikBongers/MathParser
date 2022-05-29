#pragma once
#include "pch.hpp"
#include "Range.h"

enum class ErrorId
    {
    NONE = 0,
    UNKNOWN_EXPR,
    NO_OP,

    VAR_NOT_DEF,
    FUNC_NOT_DEF,
    UNIT_NOT_DEF,
    DEFINE_NOT_DEF,

    FUNC_ARG_MIS,
    FUNC_NO_OPEN_PAR,
    FUNC_FAILED,
    UNIT_PROP_DIFF,
    CONST_REDEF,

    DATE_FRAG_NO_DATE,
    DATE_FRAG_NO_DURATION,
    DATE_INV_FRAG,
    DUR_INV_FRAG,

    VAR_EXPECTED,
    EXPECTED,
    EXPECTED_ID,

    INV_DATE_STR,
    INV_DATE_VALUE,
    INV_LIST,

    W_DIV_IMPL_MULT,
    W_POW_IMPL_MULT,
    W_ASSUMING_UNIT,
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
        Range range;
        std::vector<Error> stackTrace;

        Error() {}
        Error(ErrorId id, const Range& range);
        Error(ErrorId id, const Range& range, const std::string& arg1);
        Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2);
        Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2, const std::string& arg3);
        void to_json(std::ostringstream& sstr) const;
        bool isWarning();
    };

bool hasRealErrors(std::vector<Error>& errors);

