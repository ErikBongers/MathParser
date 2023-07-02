#pragma once
#include "Date.h"

class FormattedDateParser
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
#pragma once
