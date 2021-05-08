#pragma once
#include <string>
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

class ErrorDefs
    {
    public:
        static std::map<ErrorId, std::string> errorDefs; 
        
        static std::string getMsg(ErrorId id) { return errorDefs[id]; }
    };


class Error
    {
    public:
        ErrorId id;
        std::string errorMsg;
        int line;
        int pos;

        Error() {}
        Error(ErrorId id, va_list args=nullptr);
    };

