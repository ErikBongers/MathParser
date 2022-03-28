#pragma once
#include "tools.h"
#include <sstream>
#include <bitset>
#include <iomanip>

/// Convert number to a string or "NaN".
template <typename T>
std::string numberToString(T value, T NaN_value)
    {
    std::ostringstream numval;
    if (value == NaN_value)
        numval << "NaN";
    else
        {
        numval << std::fixed
            << std::setprecision(20)
            << value;
        }
    return numval.str();
    }
