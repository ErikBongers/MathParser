#pragma once
#include "pch.h"
#include "Range.h"

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
    NO_OP,
    DEFINE_NOT_DEF,
    INV_DATE_STR,
    DATE_FRAG_NO_DATE,
    DATE_FRAG_NO_DURATION,
    DATE_INV_FRAG,
    DUR_INV_FRAG,
    EXPECTED,
    EXPECTED_ID,
    INV_DATE_VALUE,
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

        Error() {}
        Error(ErrorId id, const Range& range);
        Error(ErrorId id, const Range& range, const std::string& arg1);
        Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2);
        Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2, const std::string& arg3);
        void to_json(std::ostringstream& sstr) const;
        bool isWarning();
    };

bool hasRealErrors(std::vector<Error>& errors);

