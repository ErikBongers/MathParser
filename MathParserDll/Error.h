#pragma once
#include "pch.hpp"
#include "Range.h"

enum class ErrorId
    {
    NONE = 0,
    UNKNOWN_EXPR,
    NO_OP,
    EOS,

    VAR_NOT_DEF,
    FUNC_NOT_DEF,
    UNIT_NOT_DEF,
    DEFINE_NOT_DEF,
    UNDEF_NOT_OK,

    FUNC_ARG_MIS,
    FUNC_NO_OPEN_PAR,
    FUNC_FAILED,
    UNIT_PROP_DIFF,
    CONST_REDEF,
    EXP_HAS_UNIT,

    DATE_FRAG_NO_DATE,
    DATE_FRAG_NO_DURATION,
    DATE_INV_FRAG,
    DUR_INV_FRAG,

    VAR_EXPECTED,
    EXPECTED,
    EXPECTED_ID,
    EXPECTED_NUMERIC_VALUE,

    INV_DATE_STR,
    INV_DATE_VALUE,
    INV_LIST,
    INV_NUMBER_STR,

    W_DIV_IMPL_MULT,
    W_POW_IMPL_MULT,
    W_ASSUMING_UNIT,
    W_VAR_IS_UNIT,
    W_UNIT_IS_VAR,
    W_VAR_IS_FUNCTION,
    W_FUNCTION_OVERRIDE,
    W_EXPLICIT_UNITS_EXPECTED,
    E_EXPLICIT_UNITS_EXPECTED,
    };


struct ErrorDef
    {
    ErrorId id = ErrorId::NONE;
    const char* name = nullptr;
    const char* message = nullptr;
    char type() const { return name[0]; }
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
        bool isWarning() const;
        bool isNone() const { return id == ErrorId::NONE; }
    };

bool hasRealErrors(std::vector<Error>& errors);

