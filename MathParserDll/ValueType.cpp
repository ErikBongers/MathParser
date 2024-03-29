#include "pch.hpp"
#include "ValueType.h"

std::string to_string(ValueType type)
    {
    return std::string(1, (char)type);
#ifdef VERBOSE
    switch (type)
        {
        case ValueType::NUMBER: return "NUMBER";
        case ValueType::TIMEPOINT: return "TIMEPOINT";
        case ValueType::CALENDAR: return "CALENDAR";
        case ValueType::DURATION: return "DURATION";
        case ValueType::LIST: return "LIST";
        case ValueType::NONE: return "NONE";
        default: return "?VALUETYPE?";
        }
#endif
    }
