#include "pch.hpp"
#include "Date.h"
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

std::string monthToString(Month m)
    {
    switch (m)
        {
        case Month::JAN: return "01";
        case Month::FEB: return "02";
        case Month::MAR: return "03";
        case Month::APR: return "04";
        case Month::MAY: return "05";
        case Month::JUN: return "06";
        case Month::JUL: return "07";
        case Month::AUG: return "08";
        case Month::SEP: return "09";
        case Month::OKT: return "10";
        case Month::NOV: return "11";
        case Month::DEC: return "12";
        default: return "??";
        }
    }

void Date::to_json(std::ostringstream& sstr) const
    {
    sstr << "{";

    sstr << "\"formatted\" : ";

    sstr << std::setfill('0') 
        << "\""
        << std::setw(4) 
        << (year == Date::EmptyYear ? 0 : year)
        << "/"
        << std::setw(2) 
        << monthToString(month)
        << "/"
        << static_cast<int>(day)
        << "\"";

    sstr << ",\"day\":\"" << (int)day << "\"";
    sstr << ",\"month\":\"" << (int)month<< "\"";
    sstr << ",\"year\":\"" << year<< "\"";

    sstr << "}";
    }

Duration Date::operator-(const Date& d2)
    {
    Duration dd;
    dd.years = this->year - d2.year;
    dd.months = (char)this->month - (char)d2.month;
    dd.days = this->day - d2.day;
    return dd;
    }

Date Date::operator+(const Duration& dur)
    {
    Date d = *this;
    if(!dur.error.isNone())
        {
        d.errors.push_back(dur.error);
        return d;
        }

    Duration duur = dur;
    duur.normalize(); //get rid of the NaN values.
    long days = d.day + duur.days;
    long months_in_days = (long)std::floor(days/30.0);
    days = (days+30)%30;

    long months= (long)d.month + duur.months + months_in_days;
    long years_in_months = (long)std::floor(months/12.0);
    months = (months+12)%12;

    long years = (long)d.year + duur.years + years_in_months;
    
    d.day = (char)days;
    d.month = (Month)months;
    d.year = years;
    return d;
    }

Date Date::operator-(const Duration& dur)
    {
    Date d = *this;
    //TODO: just call operator+ with a negative duration.
    return d;
    }

Date Date::operator+(const Number& num)
    {
    return operator+(Duration(num, range));
    }

Date Date::operator-(const Number& num)
    {
    return operator-(Duration(num, range));
    }

bool Date::isValid()
    {
    return year != EmptyYear; //a very basic check...
    }
