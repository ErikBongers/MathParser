#pragma once
#include <string>

enum class  Month : unsigned char {JAN = 1, FEB = 2, MAR = 3, APR = 4, MAY = 5, JUN = 6, JUL = 7, AUG = 8, SEP = 9, OKT = 10, NOV = 11, DEC = 12, NONE = 0};

class Date
    {
    public:
        static const char Last = 99;
        static const long EmptyYear = LONG_MIN;
        long year = EmptyYear;
        Month month = Month::NONE;
        char day = 0; //0 = none, 99 = last;
        Date() = default;
    };

class DateParser
    {
    private:
        std::string _stream;
        std::vector<std::string> slices;
    public:
        int pos=0;
        bool ymdFormat = false;
        DateParser(const std::string& str) : _stream(str) {}
        Date parse();
        void parseSlice(int sliceNo, Date& date, const std::string& slice);
        bool hasYearSlice();
        bool hasDaySlice();
        bool hasMonthSlice();
        int parseInt(const std::string& str);
        int countDateSlices();
    };
