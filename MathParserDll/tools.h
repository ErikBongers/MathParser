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

constexpr uint32_t hash(const char* data) noexcept{
    uint32_t hash = 5381;

    int i = 0;
    while(data[i] != 0)
        hash = ((hash << 5) + hash) + data[i++];

    return hash;
    }

