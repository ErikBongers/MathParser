#pragma once
#include <string>
#include "Error.h"
#include "Duration.h"
#include "Number.h"
#include "Range.h"

enum class  Month : unsigned char {JAN = 1, FEB = 2, MAR = 3, APR = 4, MAY = 5, JUN = 6, JUL = 7, AUG = 8, SEP = 9, OKT = 10, NOV = 11, DEC = 12, NONE = 0};

enum class DateFormat { YMD, DMY, MDY, UNDEFINED};
class Date
    {
    public:
        static const char Last = 99;
        static const long EmptyYear = LONG_MIN;
        long year = EmptyYear;
        Month month = Month::NONE;
        char day = 0; //0 = none, 99 = last;
        Range range;

        Date() = default;
        void to_json(std::ostringstream& sstr) const;
        std::vector<Error> errors;
        Duration operator-(const Date& d2);
        Date operator+(const Duration& dur);
        Date operator-(const Duration& dur);
        Date operator+(const Number& num);
        Date operator-(const Number& num);
        bool isValid();

    };
