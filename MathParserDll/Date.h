#pragma once
#include <string>

#ifdef __WASM__
#define C_IMP_EXP
#define C_DECL
#define CPP_IMP_EXP
#elif defined MATHPARSERDLL_EXPORTS
#define C_IMP_EXP extern "C" __declspec(dllexport)
#define C_DECL __cdecl
#define CPP_IMP_EXP __declspec(dllexport)
#else
#define C_IMP_EXP extern "C" __declspec(dllimport)
#define C_DECL __cdecl
#define CPP_IMP_EXP __declspec(dllimport)
#endif


enum class  Month : unsigned char {JAN = 1, FEB = 2, MAR = 3, APR = 4, MAY = 5, JUN = 6, JUL = 7, AUG = 8, SEP = 9, OKT = 10, NOV = 11, DEC = 12, NONE = 0};

class CPP_IMP_EXP Date
    {
    public:
        static const char Last = 99;
        static const long EmptyYear = LONG_MIN;
        long year = EmptyYear;
        Month month = Month::NONE;
        char day = 0; //0 = none, 99 = last;
        void testChrono();
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

#ifdef _DEBUG
CPP_IMP_EXP Date testDate(const char* txtDate);
#endif