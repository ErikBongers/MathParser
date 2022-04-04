#include "pch.h"
#include "Error.h"
#include "json.h"
#include <sstream>
#ifdef _MSC_VER
#include <format>
#else
#include <cassert>
#endif

Error::Error(ErrorId id, const Range& range, const std::string& arg1)
    : range(range)
    {
    this->id = id;
#ifdef _MSC_VER
    std::string buffer = std::format(ErrorDefs::get(id).message, arg1.c_str());
#else
    std::string buffer = ErrorDefs::get(id).message;
    size_t start_pos = buffer.find("{0}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg1);
#endif
    this->errorMsg = buffer.c_str();
    }

Error::Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2)
    : range(range)
    {
    this->id = id;
#ifdef _MSC_VER
    std::string buffer = std::format(ErrorDefs::get(id).message, arg1.c_str(), arg2.c_str());
#else
    std::string buffer = ErrorDefs::get(id).message;
    size_t start_pos = buffer.find("{0}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg1);

    start_pos = buffer.find("{1}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg2);

#endif
    this->errorMsg = buffer.c_str();
    }

Error::Error(ErrorId id, const Range& range, const std::string& arg1, const std::string& arg2, const std::string& arg3)
    : range(range)
    {
    this->id = id;
#ifdef _MSC_VER
    std::string buffer = std::format(ErrorDefs::get(id).message, arg1.c_str(), arg2.c_str(), arg3.c_str());
#else
    std::string buffer = ErrorDefs::get(id).message;
    size_t start_pos = buffer.find("{0}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg1);

    start_pos = buffer.find("{1}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg2);

    start_pos = buffer.find("{2}");
    if(start_pos == std::string::npos)
        assert("Probably an error message with no argument place holder.");
    buffer.replace(start_pos, 3, arg3);
#endif
    this->errorMsg = buffer.c_str();
    }

Error::Error(ErrorId id, const Range& range)
    : range(range)
    {
    this->id = id;
    this->errorMsg = ErrorDefs::get(id).message;
    }

const std::string Error::to_json()
    {
    std::ostringstream sstr;
    bool isWarning = false;
    sstr << "{";
    auto& ed = ErrorDefs::get(id);
    sstr << "\"id\":\"" << ed.name << "\",\"message\":\"" + escape_json(errorMsg) + "\"";
    isWarning = ed.name[0] == 'W';
    sstr << ",\"isWarning\":" << (isWarning ? "true":"false");
    sstr << ",\"range\":" << range.to_json();
    sstr << "}";
    return sstr.str();
    }

bool Error::isWarning()
    {
    return ErrorDefs::get(id).name[0] == 'W';
    }

std::map<ErrorId, ErrorDef> ErrorDefs::errorDefs =
    {
        {ErrorId::UNKNOWN_EXPR, {ErrorId::UNKNOWN_EXPR, "UNKNOWN_EXPR", "Unknown expression."}},
        {ErrorId::VAR_NOT_DEF, {ErrorId::VAR_NOT_DEF, "VAR_NOT_DEF", "variable {0} is not defined."}},
        {ErrorId::FUNC_NOT_DEF, {ErrorId::FUNC_NOT_DEF, "FUNC_NOT_DEF", "function {0} is not defined."}},
        {ErrorId::FUNC_ARG_MIS, {ErrorId::FUNC_ARG_MIS, "FUNC_ARG_MIS", "argument(s) for function {0} don't match."}},
        {ErrorId::FUNC_NO_OPEN_PAR, {ErrorId::FUNC_NO_OPEN_PAR, "FUNC_NO_OPEN_PAR", "Missing '(' for function {0}."}},
        {ErrorId::W_DIV_IMPL_MULT, {ErrorId::W_DIV_IMPL_MULT, "W_DIV_IMPL_MULT", "Warning: ambiguous expression: division followed by implicit multiplication."}},
        {ErrorId::W_POW_IMPL_MULT, {ErrorId::W_POW_IMPL_MULT, "W_POW_IMPL_MULT", "Warning: ambiguous expression: exponential combined with implicit multiplication."}},
        {ErrorId::UNIT_NOT_DEF, {ErrorId::UNIT_NOT_DEF, "UNIT_NOT_DEF", "unit {0} is not defined."}},
        {ErrorId::W_ASSUMING_UNIT, {ErrorId::W_ASSUMING_UNIT, "W_ASSUMING_UNIT", "Warning: not all values have a unit. Assuming same unit as the other values."}},
        {ErrorId::VAR_EXPECTED, {ErrorId::VAR_EXPECTED, "VAR_EXPECTED", "The increment (++) or decrement (--) operator can only be used on a variable."}},
        {ErrorId::UNIT_PROP_DIFF, {ErrorId::UNIT_PROP_DIFF, "UNIT_PROP_DIFF", "The units are not for the same property (lenght, temperature,...)."}},
        {ErrorId::CONST_REDEF, {ErrorId::CONST_REDEF, "CONST_REDEF", "Warning: redefinition of constant {0}."}},
        {ErrorId::W_VAR_IS_UNIT, {ErrorId::W_VAR_IS_UNIT, "W_VAR_IS_UNIT", "Warning; variable {0} overrides unit with same name."}},
        {ErrorId::W_VAR_IS_FUNCTION, {ErrorId::W_VAR_IS_FUNCTION, "W_VAR_IS_FUNCTION", "Warning; variable {0} overrides function with same name."}},
        {ErrorId::NO_OP, {ErrorId::NO_OP, "NO_OP", "No operator '{0}' defined for {1} and {2}."}},
        {ErrorId::DEFINE_NOT_DEF, {ErrorId::DEFINE_NOT_DEF, "DEFINE_NOT_DEF", "#define: option '{0}' not recognized."}},
        {ErrorId::INV_DATE_STR, {ErrorId::INV_DATE_STR, "INV_DATE_STR", "Invalid date string: '{0}'"}},
        {ErrorId::DATE_FRAG_NO_DATE, {ErrorId::DATE_FRAG_NO_DATE, "DATE_FRAG_NO_DATE", "Cannot get '{0}' fragment. Value is not a date."}},
        {ErrorId::DATE_FRAG_NO_DURATION, {ErrorId::DATE_FRAG_NO_DURATION, "DATE_FRAG_NO_DURATION", "Cannot get '{0}' fragment. Value is not a duration."}},
        {ErrorId::DATE_INV_FRAG, {ErrorId::DATE_INV_FRAG, "DATE_INV_FRAG", "Invalid fragment '{0}' for date."}},
        {ErrorId::DUR_INV_FRAG, {ErrorId::DUR_INV_FRAG, "DUR_INV_FRAG", "Invalid fragment '{0}' for duration."}},
        {ErrorId::EXPECTED, {ErrorId::EXPECTED, "EXPECTED", "Expected '{0}'."}},
        {ErrorId::INV_DATE_VALUE, {ErrorId::INV_DATE_VALUE, "INV_DATE_VALUE", "Invalid date value '{0}' for {1}."}},
    };

bool hasRealErrors(std::vector<Error>& errors)
    {
    for (auto& err : errors)
        {
        if(!err.isWarning())
            return true;
        }
    return false;
    }
