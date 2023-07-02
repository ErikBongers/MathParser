#include "pch.hpp"
#include "FormattedDateParser.h"
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

constexpr bool isDateSeparator(char c) { return c == ' ' || c == '/' || c == ',' || c == '-' ;}

Date FormattedDateParser::parse(const std::string& str, const Range& range)
    {
    _stream = str;
    this->range = range;
    Date date;
    int pos = 0;
    int start = 0;
    while (pos < _stream.length())
        {
        //skip date separators (not time!)
        while(isDateSeparator(_stream[pos]))
            {
            pos++;
            }
        start = pos;
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
        parseAnySlice(sliceNo++, date, slice);
        if(hasRealErrors(date.errors))
            {
            parseForFormat(date);
            break;
            }
        }
    if (!date.isValid())
        {
        parseForFormat(date); //TODO: might be doing this twice...
        }
    date.range = range;
    return date;
    }

void FormattedDateParser::parseForFormat(Date& date)
    {
    if(slices.size() != 3)
        return;
    //assuming slices already made.
    int daySlice=0;
    int monthSlice=0;
    int yearSlice=0;
    switch (dateFormat)
        {
        using enum DateFormat;
        case YMD:
            yearSlice = 0; monthSlice = 1; daySlice = 2; break;
        case MDY:
            monthSlice = 0; daySlice = 1; yearSlice = 2; break;
        case DMY:
            daySlice = 0; monthSlice = 1; yearSlice = 2; break;
        case UNDEFINED: 
            return;
        }

    Date tmpDate;
    if(!parseYear(tmpDate, slices[yearSlice]))
        tmpDate.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "invalid date for format."));
    if(!parseMonth(tmpDate, slices[monthSlice]))
        tmpDate.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "invalid date for format."));
    if(!parseDay(tmpDate, slices[daySlice]))
        tmpDate.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "invalid date for format."));
    if(hasRealErrors(tmpDate.errors))
        return;
    date = tmpDate;
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

bool FormattedDateParser::parseDay(Date& date, const std::string& slice)
    {
    if(slice == "last")
        {
        date.day = Date::Last;
        return true;
        }
    int n;
    if(!parseInt(slice, n))
        return false;
    if (n > 0 && n <= 31)
        {
        date.day = n;
        return true;
        }
    return false;
    }

bool FormattedDateParser::parseMonth(Date& date, const std::string& slice)
    {
    std::string lowSlice = lower(slice);
    Month month = toMonth(lowSlice);
    if(month != Month::NONE)
        {
        date.month = month;
        return true;
        }
    int n;
    if(!parseInt(slice, n))
        return false;
    if (n > 0 && n <= 12)
        {
        date.month = (Month)n;
        return true;
        }
    return false;
    }

bool FormattedDateParser::parseYear(Date& date, const std::string& slice)
    {
    int n;
    if(!parseInt(slice, n))
        return false;
    date.year = n;
    return true;
    }


void FormattedDateParser::parseAnySlice(int sliceNo, Date& date, const std::string& slice)
    {
    if (slice == "last")
        {
        if(date.day != 0)
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "multiple values for day."));
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
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "multiple values for month."));
        }
    //from here on, it should be all numbers.
    int n;
    if(!parseInt(slice, n))
        {
        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "invalid numeric value."));
        return;
        }
    if(n < 0)
        {
        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "invalid numeric value."));
        return;
        }
    //what number do we have?
    if (n > 31)
        {
        if(date.year != Date::EmptyYear)
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "multiple values for year."));
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
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "values could be month or year."));
            return;
            }
        if(date.day != 0)
            {
            date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "multiple values for day."));
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
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "not clear which value is day or month."));
                        }
                    return;
                    }
                if (hasDaySlice())
                    {
                    if(date.month == Month::NONE)
                        date.month = (Month)n;//should not fail as n <= 12
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "not clear which value is day or month."));
                        }
                    return;
                    }
                if (countSameDateValues(n) >=2)
                    {
                    date.day = n;
                    date.month = (Month)n;
                    return;
                    }
                date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "not clear which value is day or month."));
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
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "assuming ymd format, but day is already filled."));
                        return;
                        }
                    }
                else if (sliceNo == 1)
                    {
                    if(date.month == Month::NONE)
                        date.month = (Month)n;//should not fail as n <= 12
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "assuming ymd but month is already filled."));
                        return;
                        }
                    }
                else if(sliceNo == 2)
                    {
                    if(date.day == 0)
                        date.day = n;
                    else
                        {
                        date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "assuming ymd format, but day is already filled."));
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
                date.errors.push_back(Error(ErrorId::INV_DATE_STR, range, "not clear which value is day or month."));
                return;
                }
            }
        }
    }


bool FormattedDateParser::hasYearSlice()
    {
    for (auto& slice : slices)
        {
        int n;
        if(parseInt(slice, n))
            if(n > 31)
                return true;
        }
    return false;
    }

bool FormattedDateParser::hasDaySlice()
    {
    int i = 0;
    for (auto& slice : slices)
        {
        if(slice == "last")
            return true;
        int n;
        if(parseInt(slice, n))
            if(n > 12 && n <= 31)
                return true;
        i++;
        }
    return false;
    }

bool FormattedDateParser::hasMonthSlice()
    {
    for (auto& slice : slices)
        {
        auto lowSlice = lower(slice);
        if(toMonth(lowSlice)!= Month::NONE)
            return true;
        }
    return false;
    }

int FormattedDateParser::countSameDateValues(int valueToCount)
    {
    int i = 0;
    for (auto& slice : slices)
        {
        int n;
        parseInt(slice, n); //TODO: test return value?
        if(n == valueToCount)
            i++;
        }
    return i;
    }

bool FormattedDateParser::parseInt(const std::string& str, int& result)
    {
    int n = 0;
    for (auto& c : str)
        {
        if(c >= '0' && c <= '9')
            n = n*10 + c-'0';
        else
            return false;
        }
    result = n;
    return true;
    }

int FormattedDateParser::countDateSlices()
    {
    return (int)slices.size();//TODO: exclude time slices.
    }
