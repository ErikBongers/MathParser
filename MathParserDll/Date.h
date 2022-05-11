#pragma once
#include <string>
#include "Error.h"
#include "Duration.h"
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
        Date() = default;
        void to_json(std::ostringstream& sstr) const;
        std::vector<Error> errors;
        Duration operator-(const Date& d2);
    };

class DateParser
    {
    private:
        std::string _stream;
        std::vector<std::string> slices;
        Range range;
        void parseForFormat(Date& date);
        bool ymdFormat = false;
        bool parseDay(Date& date, const std::string& slice);
        bool parseMonth(Date& date, const std::string& slice);
        bool parseYear(Date& date, const std::string& slice);
        bool parseInt(const std::string& str, int& result);
    public:
        DateFormat dateFormat = DateFormat::UNDEFINED;
        Date parse(const std::string& str, const Range& range);
        void parseAnySlice(int sliceNo, Date& date, const std::string& slice);
        bool hasYearSlice();
        bool hasDaySlice();
        bool hasMonthSlice();
        int countSameDateValues(int valueToCount);
        int countDateSlices();
    };
