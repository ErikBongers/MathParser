#include "pch.h"
#include "Duration.h"
#include <sstream>
#include <iomanip>
#include "tools.h"

std::string Duration::to_json()
    {
    std::ostringstream sstr;
    sstr << "{";

    sstr << "\"years\":";
    sstr << "\"" << numberToString(years, EmptyYears) << "\"";

    sstr << ",\"months\":";
    sstr << "\"" << numberToString(months, EmptyMonths) << "\"";

    sstr << ",\"days\":";
    sstr << "\"" << numberToString(days, EmptyDays) << "\"";

    sstr << "}";
    return sstr.str();
    }

void Duration::normalize()
    {
    double months_in_days = std::floor(days/30.0);
    days = (days+30)%30;
    months+=(long)months_in_days;
    double years_in_months = std::floor(months/12.0);
    months = (months+12)%12;
    years +=(long)years_in_months;
    }
