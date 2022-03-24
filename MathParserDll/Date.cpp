#include "pch.h"
#include "Date.h"
#include <chrono>
#include <algorithm>

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
        parseSlice(sliceNo++, date, slice);
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
            ;//error!
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
            ;//error: month is already filled.
        }
    //from here on, it should be all numbers.
    int n = parseInt(slice);
    if(n < 0)
        return;//error in number;
    //what number do we have?
    if (n > 31)
        {
        if(date.year != Date::EmptyYear)
            ;//error
        else
            {
            date.year = n;
            if(sliceNo == 0)
                ymdFormat = true;
            }
        }
    else if (n > 12)
        {
        if(date.day != 0)
            ;//error
        else
            date.day = n;
        }
    else //n <= 12
        {
        if(countDateSlices() == 3)
            {
            //slice could be day, month, year
            if (hasYearSlice())
                {
                if (hasMonthSlice())
                    {
                    date.day = n;
                    return;
                    }
                if (hasDaySlice())
                    {
                    date.month = (Month)n;
                    return;
                    }
                }
            if(ymdFormat)
                {
                // ymd format
                if(sliceNo == 0)
                    {
                    if(date.year == Date::EmptyYear)
                        date.year = n;
                    else
                        ;//error: assuming ymd format, but day is already filled.
                    }
                else if (sliceNo == 1)
                    {
                    if(date.month == Month::NONE)
                        date.month = (Month)n;//should not fail as n <= 12
                    else
                        ;//error: assuming ymd but month is already filled.
                    }
                else if(sliceNo == 2)
                    {
                    if(date.day == 0)
                        date.day = n;
                    else
                        ;//error: assuming ymd format, but day is already filled.
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
                ; //error: slice could be day or month
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
