#include "pch.hpp"
#include "Duration.h"
#include "Number.h"
#include <sstream>
#include <iomanip>
#include "tools.h"

void Duration::to_json(std::ostringstream& sstr) const
    {
    sstr << "{";

    sstr << "\"years\":";
    sstr << "\"" << numberToString(years, EmptyYears) << "\"";

    sstr << ",\"months\":";
    sstr << "\"" << numberToString(months, EmptyMonths) << "\"";

    sstr << ",\"days\":";
    sstr << "\"" << numberToString(days, EmptyDays) << "\"";

    sstr << "}";
    }

void Duration::normalize()
    {
    if(days == Duration::EmptyDays)
        days = 0;
    if(months == Duration::EmptyMonths)
        months = 0;
    if(years == Duration::EmptyYears)
        years = 0;
    double months_in_days = std::floor(days/30.0);
    days = (days+30)%30;
    months+=(long)months_in_days;
    double years_in_months = std::floor(months/12.0);
    months = (months+12)%12;
    years +=(long)years_in_months;
    }

double Duration::to_days() const
    {
    double ytod = years == EmptyYears ? 0 : years * 365.2425;
    double mtod = months == EmptyMonths ? 0 : months * 365.2425 / 12;
    double dtod = days == EmptyDays ? 0 : days;
    return ytod + mtod + dtod;
    }

Duration::Duration(const Number& num, const Range& range)
    : range(range)
    {
    if (num.unit.id == "days")
        {
        this->days = (long)num.to_double();
        }
    else if (num.unit.id == "months")
        {
        this->months = (long)num.to_double();
        }
    else if (num.unit.id == "years")
        {
        this->years = (long)num.to_double();
        }
    else
        error = Error(ErrorId::DUR_INV_FRAG, range, num.unit.id); //TODO: unit id can be empty.
    }

Duration Duration::operator+(const Number& num) const
    {
    Duration dur = *this;
    if (num.unit.id == "days")
        {
        dur.days += (long)num.to_double();
        }
    else if (num.unit.id == "months")
        {
        dur.months += (long)num.to_double();
        }
    else if (num.unit.id == "years")
        {
        dur.years += (long)num.to_double();
        }
    else
        dur.error = Error(ErrorId::DUR_INV_FRAG, range, num.unit.id); //TODO: unit id can be empty.
    return dur;
    }

Duration Duration::operator-(const Number& num) const
    {
    Duration dur = *this;
    if (num.unit.id == "days")
        {
        dur.days -= (long)num.to_double();
        }
    else if (num.unit.id == "months")
        {
        dur.months -= (long)num.to_double();
        }
    else if (num.unit.id == "years")
        {
        dur.years -= (long)num.to_double();
        }
    else
        dur.error = Error(ErrorId::DUR_INV_FRAG, range, num.unit.id); //TODO: unit id can be empty.
    return dur;
    }
