#include "pch.h"
#include "Date.h"
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

constexpr bool isDateSeparator(char c) { return c == ' ' || c == '/' || c == ',' || c == '-' ;}

Date DateParser::parse()
    {
    Date date;
    //TODO: check if a preference has been set for day first or month first...
    int pos = 0;
    int start = 0;
    while (pos < _stream.length())
        {
        //skip date separators (not time!)
        while(isDateSeparator(_stream[pos]))
            {
            pos++;
            }
        start = pos;;
        //skip to next separator
        while(pos < _stream.length() && !isDateSeparator(_stream[pos]))
            {
            pos++;
            }
        slices.push_back(_stream.substr(start, pos-start));
        pos++;
        }
    
    int sliceNo = 0;
    for(auto& slice: slices)
        {
        parseSlice(sliceNo++, date, slice);
        if(hasRealErrors(date.errors))
            break;
        }
    date.line = line;
    date.pos = pos;
    return date;
    }

//Note: only works for ASCII and probably not very efficient.
std::string lower(const std::string& str)
    {
    std::string low;
    for (auto& c : str)
        {
        low += std::tolower(c);
        }
    return low;
    }

Month toMonth(const std::string& str)
    {
    if(str == "jan" || str == "january")
        return Month::JAN;
    if(str == "feb" || str == "february")
        return Month::FEB;
    if(str == "mar" || str == "march")
        return Month::MAR;
    if(str == "apr" || str == "april")
        return Month::APR;
    if(str == "may")
        return Month::MAY;
    if(str == "jun" || str == "june")
        return Month::JUN;
    if(str == "jul" || str == "july")
        return Month::JUL;
    if(str == "aug" || str == "august")
        return Month::AUG;
    if(str == "sep" || str == "september")
        return Month::OKT;
    if(str == "okt" || str == "oktober")
        return Month::NOV;
    if(str == "nov" || str == "november")
        return Month::DEC;
    if(str == "dec" || str == "december")
        return Month::JAN;
    return Month::NONE;
    }

void DateParser::parseSlice(int sliceNo, Date& date, const std::string& slice)
    {
    if (slice == "last")
        {
        if(date.day != 0)
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "multiple values for day."));
        else
            date.day = Date::Last;
        return;
        }
    std::string lowSlice = lower(slice);
    Month month = toMonth(lowSlice);
    if(month != Month::NONE)
        {
        if(date.month == Month::NONE)
            { 
            date.month = month;
            return;
            }
        else
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "multiple values for month."));
        }
    //from here on, it should be all numbers.
    int n = parseInt(slice);
    if(n < 0)
        {
        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "invalid numeric value."));
        return;
        }
    //what number do we have?
    if (n > 31)
        {
        if(date.year != Date::EmptyYear)
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "multiple values for year."));
        else
            {
            date.year = n;
            if(sliceNo == 0)
                ymdFormat = true;
            }
        }
    else if (n > 12) //not a month, could be day or year.
        {
        if (countDateSlices() >= 3 && !hasYearSlice())
            {
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "values could be month or year."));
            return;
            }
        if(date.day != 0)
            {
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "multiple values for day."));
            return;
            }
        date.day = n;
        }
    else //n <= 12
        {
        if(countDateSlices() == 3)
            {
            if (countSameDateValues(n) == 3)
                {
                date.day = n;
                date.month = (Month)n;
                date.year = n;
                return;
                }
            //slice could be day, month, year
            if (hasYearSlice())
                {
                if (hasMonthSlice())
                    {
                    if(date.day == 0)
                        date.day = n;
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "not clear which value is day or month."));
                        }
                    return;
                    }
                if (hasDaySlice())
                    {
                    if(date.month == Month::NONE)
                        date.month = (Month)n;//should not fail as n <= 12
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "not clear which value is day or month."));
                        }
                    return;
                    }
                if (countSameDateValues(n) >=2)
                    {
                    date.day = n;
                    date.month = (Month)n;
                    return;
                    }
                date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "not clear which value is day or month."));
                }
            if(ymdFormat)
                {
                // ymd format
                if(sliceNo == 0)
                    {
                    if(date.year == Date::EmptyYear)
                        date.year = n;
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "assuming ymd format, but day is already filled."));
                        return;
                        }
                    }
                else if (sliceNo == 1)
                    {
                    if(date.month == Month::NONE)
                        date.month = (Month)n;//should not fail as n <= 12
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "assuming ymd but month is already filled."));
                        return;
                        }
                    }
                else if(sliceNo == 2)
                    {
                    if(date.day == 0)
                        date.day = n;
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "assuming ymd format, but day is already filled."));
                        return;
                        }
                    }
                }
            }
        else if(countDateSlices() == 2)
            {
            if (hasDaySlice())
                {
                date.month = (Month)n;
                return;
                }
            else if (hasMonthSlice())
                {
                date.day = n;
                return;
                }
            else
                {
                date.errors.push_back(Error(ErrorId::INV_DATE_STR, line, pos, "not clear which value is day or month."));
                return;
                }
            }
        }
    }

bool DateParser::hasYearSlice()
    {
    for (auto& slice : slices)
        {
        int n = parseInt(slice);
        if(n > 31)
            return true;
        }
    return false;
    }

bool DateParser::hasDaySlice()
    {
    int i = 0;
    for (auto& slice : slices)
        {
        if(slice == "last")
            return true;
        int n = parseInt(slice);
        if(n > 12 && n <= 31)
            return true;
        i++;
        }
    return false;
    }

bool DateParser::hasMonthSlice()
    {
    for (auto& slice : slices)
        {
        auto lowSlice = lower(slice);
        if(toMonth(lowSlice)!= Month::NONE)
            return true;
        }
    return false;
    }

int DateParser::countSameDateValues(int valueToCount)
    {
    int i = 0;
    for (auto& slice : slices)
        {
        int n = parseInt(slice);
        if(n == valueToCount)
            i++;
        }
    return i;
    }

int DateParser::parseInt(const std::string& str)
    {
    int n = 0;
    for (auto& c : str)
        {
        if(c >= '0' && c <= '9')
            n = n*10 + c-'0';
        else
            return -1;
        }
    return n;
    }

int DateParser::countDateSlices()
    {
    return (int)slices.size();//TODO: exclude time slices.
    }

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


std::string Date::to_json()
    {
    std::ostringstream sstr;
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

    sstr << ", \"day\" : \"" << (int)day << "\"";
    sstr << ", \"month\" : \"" << (int)month<< "\"";
    sstr << ", \"year\" : \"" << year<< "\"";

    sstr << "}";
    return sstr.str();
    }

Duration Date::operator-(const Date& d2)
    {
    Duration dd;
    dd.years = this->year - d2.year;
    dd.months = (char)this->month - (char)d2.month;
    dd.days = this->day - d2.day;
    return dd;
    }
