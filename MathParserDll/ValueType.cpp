#include "pch.h"
#include "ValueType.h"

std::string to_string(ValueType type)
    {
    switch (type)
        {
        case ValueType::NUMBER: return "NUMBER";
        case ValueType::TIMEPOINT: return "TIMEPOINT";
        case ValueType::CALENDAR: return "CALENDAR";
        case ValueType::DURATION: return "DURATION";
        case ValueType::NONE: return "NONE";
        default: return "?VALUETYPE?";
        }
    }
