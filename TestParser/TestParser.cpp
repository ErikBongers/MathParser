#include "pch.h"
#include "CppUnitTest.h"
#include "../MathParserDll/api.h"
#include "../MathParserDll/FormattedDateParser.h"
#include "../MathParserDll/Parser.h"
#include "../MathParserDll/Function.h"
#include "../MathParserDll/Scope.h"

#ifdef VERBOSE
const std::string VALTYPE_NUMBER = "NUMBER";
const std::string VALTYPE_TIMEPOINT = "TIMEPOINT";
const std::string VALTYPE_CALENDAR = "CALENDAR";
const std::string VALTYPE_DURATION = "DURATION";
const std::string VALTYPE_LIST = "LIST";
const std::string VALTYPE_NONE = "NONE";
#else
const std::string VALTYPE_NUMBER = "N";
const std::string VALTYPE_TIMEPOINT = "T";
const std::string VALTYPE_CALENDAR = "C";
const std::string VALTYPE_DURATION = "D";
const std::string VALTYPE_LIST = "L";
const std::string VALTYPE_NONE = "_";
#endif


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nlohmann;
namespace TestParser
{
    TEST_CLASS(TestParser)
    {
    public:
        TEST_METHOD(TestConstants)
            {
            assertResult("trunc(PI);", 3);
            }
        TEST_METHOD(TestFunctions)
            {
            //basic function call
            assertResult(R"CODE(
a=3;
function hundred(a)
  {
  a*100;
  }
hundred(a);
                         )CODE"
                         , 300);

            //outer scope var not changed
            assertResult(R"CODE(
a=3;
function hundred(a)
  {
  a*100;
  }
hundred(a);
a;
                         )CODE"
                         , 3);
            //inner scope var not visible outside.
            assertError(R"CODE(
a=3;
function hundred(a)
  {
  x = 123;
  a*100;
  }
hundred(a);
x;
                         )CODE"
                         , "VAR_NOT_DEF");

            //nested functions
            assertResult(R"CODE(
a=3;
function hundred(a)
  {
  function getFactor(z) { z*5; }
  a*getFactor(20);
  }
hundred(a);
                         )CODE"
                        , 300);

            }

        TEST_METHOD(TestDurations)
            {
            assertDuration("duur=2 days, 3 months", 2, 3);
            assertDuration("'Jan 12, 2022'-'Jan 11, 2022';", 1);
            assertDate("'01 jan 2022'+2days", 3, 1, 2022);
            assertError("'01 jan 2022'+2", "DUR_INV_FRAG");
            assertDate(R"CODE(
dat='Jan 12, 2022'; 
duur=2 days, 3 months;
dat+duur;
                    )CODE", 14, 4, 2022);
        assertDate(R"CODE(
dat='Jan 12, 2022'; 
duur=360 days, 0 months; //calculated year of 30*12 days
dat+duur;
                    )CODE", 12, 1, 2023);
            assertDuration("duur=2 days, 3 months; duur + 2 days", 4, 3);
            }

        TEST_METHOD(TestDates)
            {
            testDateString("11/11/11", 11, 11, 11);
            testDateString("2022/12/13", 13, 12, 2022);
            assertDate("'2022/12/13'", 13, 12, 2022);

            testDateString("2022,12,13", 13, 12, 2022);
            testDateString("2022-12-13", 13, 12, 2022);
            testDateString("Jan 12, 2022", 12, 1, 2022);
            testDateString("12 28 2022", 28, 12, 2022);
            testDateString("28 12 2022", 28, 12, 2022);
            testDateString("2022 12 28", 28, 12, 2022);
            testDateString("2022 28 12", 28, 12, 2022);
            testDateString("28 2022 12", 28, 12, 2022);
            testDateString("12 2022 28", 28, 12, 2022);
            testDateString("2022 last 1", 99, 1, 2022);
            testDateString("last 1", 99, 1, Date::EmptyYear);
            testDateString("26 1", 26, 1, Date::EmptyYear);
            testDateString("feb 1", 1, 2, Date::EmptyYear);
            
            assertError("'2022 2 1'", "INV_DATE_STR");
            assertDate("#define ymd\n '2022 2 1'", 1, 2, 2022);
            assertDate("#define ymd; '2022 2 1'", 1, 2, 2022); //#define ends with either a NL or a ;

            // date via lists:
            assertDate("d = date(2022, 12, 13)", 13, 12, 2022);
            }

        TEST_METHOD(TestDefines)
            {
//            assertResult("#define   date_units   short_date_units\n a=3s;", 3, "s");
//            assertError("#define sdfsdf", "DEFINE_NOT_DEF");
//            assertError("#undef trig\n sin(30deg)", "FUNC_NOT_DEF");
//            assertError("#undef all\n sin(30deg)", "FUNC_NOT_DEF");
//            assertError("#undef all\n max(30, 40)", "FUNC_NOT_DEF");
//            assertError("#undef all\n now()", "FUNC_NOT_DEF");
//            assertResult(R"CODE(
//#undef trig
//#define trig
//sin(30deg);
//                        )CODE"
//                         , 0.5);
//            assertResult(R"CODE(
//#undef arithm
//#define arithm
//max(30, 20);
//                        )CODE"
//                         , 30);
//            assertResult(R"CODE(
//#undef all
//#define all
//sin(30deg);
//                        )CODE"
//                         , 0.5);
            assertResult(R"CODE(
#undef date
#define date
d=now() - now();
d.years + d.months + d.days;

                        )CODE"
                         , 0);
//            assertError(R"CODE(
//#define strict
//sin(3);
//                        )CODE"
//                        , "W_EXPLICIT_UNITS_EXPEDTED");
            }

        TEST_METHOD(TestPrecedence)
            {
            assertResult("1+2*3^4;", 163);
            assertResult("4-3-2+1;", 0);
            assertResult("12/3*2;", 8);
            }

        TEST_METHOD(TestImplicitMult)
            {
            assertResult("a=2; 3 a;", 6);
            assertResult("a=2; 3(4a);", 24);
            assertResult("a=2; a(3(4a));", 48);
            assertResult("m=2; 2m;", 4);
            assertError("m=2; 2m;", "W_UNIT_IS_VAR");
            }

        TEST_METHOD(TestUnary)
            {
            assertResult("a=-1;", -1); //not really a unary, but for completeness sake
            assertResult("a=-(1);", -1);
            assertResult("a=-sin(30deg);", -0.5);
            assertResult("a=1; a=-a;", -1);
            }

        TEST_METHOD(TestCalls)
            {
            assertResult("sin(30deg)", 0.5);
            assertResult("sin(30deg)+1", 1.5);
            assertResult("abs(-sin(30deg))", 0.5);
            assertResult("|-sin(30deg)|", 0.5);
            assertResult("a=-123;|a|", 123);
            assertResult("max(-sin(30deg), 3, 9999)", 9999);
            }

        TEST_METHOD(TestEcho)
            {
            //assertOutput("a=1;", 1, "", "");
            //assertOutput("a=1; //comment", 1, "", "");
            assertOutput("1; !//comment", 1, "", "comment");
            //assertOutput("!a=1;", 1, "a=1;", "");
            //assertOutput("!a=1; !//comment", 1, "a=1;", "comment");
            //assertOutput("!!a=1; //comment", 1, "a=1;", "comment");
            assertResult(R"CODE(
!///
a=3;
///!
a;
                         )CODE"
                         , 3);

            }

        TEST_METHOD(TestBlocks)
            {
            assertResult(R"CODE(
#define decimal_dot
theDot = '1,234.56';
{
#define decimal_comma
theDot = '1.234,56';
}
theDot = '1,234.56';
                         )CODE"
                         , 1234.56);

            assertError(R"CODE(
a = 1;
{
b = a;
c=3;
}
d=c;
                         )CODE"
                        , "VAR_NOT_DEF");

            }
        TEST_METHOD(TestNumberFormats)
            {
            assertResult(".123", 0.123);
            assertResult("123.456", 123.456);
            assertResult("'1,234.56'", 1234.56);
            assertResult("#define decimal_comma \n '1.234,56'", 1234.56);
            assertResult("123_456", 123456);
            assertResult("-123_456", -123456);
            assertResult("0b111_1011", 123, "", "", "BIN");
            assertResult("0x1E240", 123456, "", "", "HEX");
            assertResult("0b111_1011C.dec", 123, "C", "", "DEC");
            assertResult("(0b111_1011.)", 123, "", "", "BIN");
            assertResult("(0b111_1011C.).dec", 123, "", "", "DEC");
            assertResult(R"CODE(
hx1=0x0001;
hx2=0x7a;
hx3=hx1+hx2;
                        )CODE"
                        , 123, "", "", "HEX");
            }

        TEST_METHOD(TestExponents)
            {
            assertExponent("123E10 + 234E10", 357, 10);
            assertExponent("234E10 - 123E10", 111, 10);
            assertExponent("123E10 * 2E10", 246, 20);
            assertExponent("246E10 / 2E10", 123, 0);
            assertExponent("1E-2", 1, -2);
            //combined with units:
            assertExponent("1E2mm.cm", 0.1, 2, "cm");
            assertExponent("1E3m + 1km", 2, 3, "m");
            }

        TEST_METHOD(TestNameConflicts)
            {
            assertResult("  km=123                    ", 123, "", "W_VAR_IS_UNIT");
            assertResult("  sin=123                   ", 123, "", "W_VAR_IS_FUNCTION");
            }

        TEST_METHOD(TestErrorsAndWarnings)
            {
            assertResult("a=3; 12/2a", 2, "", "W_DIV_IMPL_MULT");
            assertResult("a=3; max(12/2a, 0)", 2, "", "W_DIV_IMPL_MULT");
            }


        TEST_METHOD(TestIncompleteProgram)
            {
            assertError("   a=12/             ", "EOS");
            assertError("   a=             ", "EOS");
            assertError("   {             ", "EXPECTED");
            }

        TEST_METHOD(TestAssignStatements)
            {
            assertResult("  a=3;a.=km               ", 3, "km");
            assertResult("  a=3km; a.=;             ", 3, "");
            assertResult("  a=3;a*=2                ", 6, "");
            assertResult("  a=3;a*=2km              ", 6, "km");
            assertResult("  a=3km; a*=2;            ", 6, "km");
            assertResult("  a=3;a+=2                ", 5, "");
            assertResult("  a=3km; a+=2;            ", 5, "km", "W_ASSUMING_UNIT");
            assertResult("  a=3; a+=2km;            ", 5, "km", "W_ASSUMING_UNIT");
            assertResult("  dec=123;                ", 123, ""); // dec should not conflict with the dec() function.
            }

        TEST_METHOD(TestIdNames)
            {
            assertResult("  a=3;                    ", 3, "");
            assertResult("  a_b=3;                  ", 3, "");
            assertResult("  _a_b=3;                 ", 3, "");
            }

        TEST_METHOD(TestUnits)
            {
            assertResult("  10km+5m                 ", 10.005, "km");
            assertResult("  (10km+5m).              ", 10.005, "");
            assertResult("  (10km.)                 ", 10, "");
            assertResult("  (10km+5m).m             ", 10005, "m");
            assertResult("  10kg.g.N.lb.kg          ", 10, "kg");
            assertResult("  max(10,20)km            ", 20, "km");
            assertResult("  max(10m,2000m)km        ", 2, "km");
            assertResult("  a=10;a.km               ", 10, "km");
            assertResult("  a=10;(a).km             ", 10, "km");
            assertResult("  a=10;(a)km              ", 10, "km");
            assertError("   a=10g;(a)km             ", "UNIT_PROP_DIFF");
            assertError("   a=10g;a.km              ", "UNIT_PROP_DIFF");
            assertResult("  a=10g;(a.)km            ", 10, "km");
            assertError("   max(1kg, 4C)            ", "UNIT_PROP_DIFF");
            assertResult("  max(1kg, 4g)            ", 1, "kg");
            assertResult("  (PI)kg                  ", 3.14159265358979311600, "kg");
            assertResult("  1m*2                    ", 2, "m");
            assertResult("  1*2m                    ", 2, "m");
            assertResult("  a=2; a.=m;              ", 2, "m");
            assertError("   a=1; a b;               ", "UNIT_NOT_DEF");
            }

        TEST_METHOD(TestRanges)
            {
            assertErrorRange("a;", 0, 0, 0, 1);
            assertErrorRange(" a ;", 0, 1, 0, 2);
            //assertErrorRange(" _a_ ;", 0, 1, 0, 4);

            assertErrorRange(" sinx() ;", 0, 1, 0, 5);

            }

        TEST_METHOD(TestEchoText)
            {
            assertText("! \n  x=5; \n \n \n", "x=5");
            }

        struct Error
            {
            std::string message;
            };

        struct Result
            {
            std::string comment;
            std::vector<Error> errors;
            std::string id;
            bool mute;
            bool onlyComment;
            std::string text;
            std::string unit;
            double value;
            };

        struct JsonAndString
            {
            json j;
            std::string str;
            };

        void assertExponent(const char* stmt, double expectedNumber, int expectedExponent, const std::string expectedUnit = "")
            {
            assertResult(stmt, expectedNumber, expectedUnit, "", "DEC", expectedExponent);
            }

        void assertOutput(const char* stmt, double expectedResult, const std::string text, const std::string comment)
            {
            std::string msg;

            auto result = assertResult(stmt, expectedResult);
            //text
            msg = std::format("\"{0}\" has incorrect comment\"{1}\", which should be \"{2}\"", stmt, (const std::string)result.j["text"], (const std::string)result.j["text"]);
            Assert::AreEqual(text, (const std::string)result.j["text"], toWstring(msg).c_str());
            //comment
            msg = std::format("\"{0}\" has incorrect comment\"{1}\", which should be \"{2}\"", stmt, (const std::string)result.j["comment"], (const std::string)result.j["comment"]);
            Assert::AreEqual(comment, (const std::string)result.j["comment"], toWstring(msg).c_str());
            }

        void assertErrors(JsonAndString& result, const char* stmt, const std::string errorId = "")
            {
            std::string msg;
            if(hasErrors(result))
                {
                if (errorId == "")
                    {
                    if(hasRealErrors(result))
                        {
                        msg = std::format("\"{0}\" has errors: {1}", stmt, (const std::string)(result.j["errors"][0]["id"]));
                        Assert::Fail(toWstring(msg).c_str());
                        }
                    }
                else
                    {
                    if(!hasError(result, errorId))
                        {
                        msg = std::format("\"{0}\" did not report error: {1}", stmt, errorId);
                        Assert::Fail(toWstring(msg).c_str());
                        }
                    }
                }
            else if(errorId != "")
                {
                msg = std::format("\"{0}\" did not report error: {1}", stmt, errorId);
                Assert::Fail(toWstring(msg).c_str());
                }
            }

        void assertDate(const char* stmt, int day = 0, int month = 0, long year = 0, const std::string errorId = "")
            {
            std::string msg;

            auto result = parseSingleResult(stmt);
            logJson(result);
            assertErrors(result, stmt, errorId);
            if(result.j["type"] !=  VALTYPE_TIMEPOINT)
                {
                std::string strType = result.j["type"];
                msg = std::format("Expected TIMEPOINT, found {0}", strType);
                Assert::Fail(toWstring(msg).c_str());
                }
            json date = result.j["date"];
            std::string strDay = date["day"]; int d = std::stoi(strDay);
            std::string strMonth = date["month"]; int m = std::stoi(strMonth);
            std::string strYear = date["year"]; long y = std::stol(strYear);
            Assert::AreEqual(day, d);
            Assert::AreEqual(month, m);
            Assert::AreEqual(year, y);
            }

        void assertDuration(const char* stmt, int days = 0, int months = 0, long years = 0, const std::string errorId = "")
            {
            std::string msg;

            auto result = parseSingleResult(stmt);
            logJson(result);
            assertErrors(result, stmt, errorId);
            json date = result.j["duration"];
            int d = 0;
            int m = 0;
            long y = 0;
            std::string strDays = date["days"]; 
            if(strDays != "NaN")
             d = std::stoi(strDays);
            std::string strMonths = date["months"];
            if(strMonths != "NaN")
                m = std::stoi(strMonths);
            std::string strYears = date["years"]; 
            if(strYears != "NaN")
                y = std::stol(strYears);
            Assert::AreEqual(d, days);
            Assert::AreEqual(m, months);
            Assert::AreEqual(y, years);
            }

        JsonAndString assertResult(const char* stmt, double expectedResult, const std::string expectedUnit = "", const std::string errorId = "", const std::string expectedFormat = "DEC", int expectedExponent = 0)
            {
            std::string msg;

            auto result = parseSingleResult(stmt);
            logJson(result);
            assertErrors(result, stmt, errorId);
            json number = result.j["number"];
            if(number["exp"] == 0)
                {
                //value
                std::string value = number["val"];
                double d = std::stod(value);
                Assert::AreEqual(std::round(expectedResult*10000)/10000, std::round(d*10000)/10000);
                }
            else
                {
                //number
                std::string significand = number["sig"];
                double d = std::stod(significand);
                Assert::AreEqual(expectedResult, d);
                //exponent
                int e = number["exp"];
                msg = std::format("\"{0}\" has incorrect exponent {1}, which should be {2}", stmt, (int)number["exp"], expectedExponent);
                Assert::AreEqual(expectedExponent, (int)number["exp"], toWstring(msg).c_str());
                }
            //unit
            msg = std::format("\"{0}\" has incorrect unit {1}, which should be {2}", stmt, (const std::string)number["u"], expectedUnit);
            Assert::AreEqual(expectedUnit, (const std::string)number["u"], toWstring(msg).c_str());
            //format
            msg = std::format("\"{0}\" has incorrect format {1}, which should be {2}", stmt, (const std::string)number["fmt"], expectedFormat);
            Assert::AreEqual(expectedFormat, (const std::string)number["fmt"], toWstring(msg).c_str());
            return result;
            }

        void assertText(const char* stmt, const char* text)
            {
            std::string msg;

            auto result = parseSingleResult(stmt);
            logJson(result);
            assertErrors(result, stmt, "");
            //text
            Assert::AreEqual(std::string(text), result.j["text"].get<std::string>(), toWstring("Text of echo statement is incorrect.").c_str());
            }

        void assertErrorRange(const char* stmt, unsigned int startLine, unsigned int startPos, unsigned int endLine, unsigned int endPos)
            {
            std::string msg;
            auto result = parseSingleResult(stmt);
            logJson(result);
            if (hasErrors(result))
                {
                auto errors = result.j["errors"];
                auto range = errors[0]["range"];
                double sLine = range["startLine"].get<double>();
                double sPos = range["startPos"].get<double>();
                double eLine = range["endLine"].get<double>();
                double ePos= range["endPos"].get<double>();
                if(startLine != sLine || startPos != sPos || endLine != eLine || endPos != ePos)
                    {
                    msg = std::format("\"{0}\" did not report a correct range", stmt);
                    Assert::Fail(toWstring(msg).c_str());
                    }

                return;
                }                    
            msg = std::format("\"{0}\" did not report any errors", stmt);
            Assert::Fail(toWstring(msg).c_str());
            }

        void assertError(const char* stmt, const std::string errorId)
            {
            std::string msg;
            auto result = parseSingleResult(stmt);
            logJson(result);
            if (hasError(result, errorId))
                {
                return;
                }                    
            msg = std::format("\"{0}\" did not report error: {1}", stmt, errorId);
            Assert::Fail(toWstring(msg).c_str());
            }

        bool hasErrors(JsonAndString& result)
            {
            auto errors = result.j["errors"];
            return errors.size() > 0;
            }

        bool hasRealErrors(JsonAndString& result)
            {
            auto errors = result.j["errors"];
            for (auto& error : errors)
                {
                auto strId = error["id"].get<std::string>();
                if(strId[0] != 'W')
                    return true;
                }
            return false;
            }

        bool hasError(JsonAndString& result, const std::string& errorId)
            {
            auto errors = result.j["errors"];
            for (auto& error : errors)
                {
                if(error["id"] == errorId)
                    return true;
                }
            return false;
            }

        void logJson(JsonAndString& result)
            {
            std::ostringstream oss;
            oss << result.j.dump(2) << std::endl;
            Logger::WriteMessage(oss.str().c_str());
            }

        JsonAndString  parseSingleResult(const char* stmt)
            {
            int resLen = parse(stmt);
            char* result = new char[resLen];
            getResult(result, resLen);
            json j = json::parse(result);
            std::string strResult = result;
            delete[] result;
            return {j["result"].back(), strResult}; //back = last element, as opposed to front()
            }

        std::wstring toWstring(const std::string& s)
            {
            std::wstringstream cls;
            cls << s.c_str();
            return cls.str();
            }

        void testDateString(const char* txtDate, int day = 0, int month = 0, long year = 0)
            {
            FormattedDateParser parser;
            Date date = parser.parse(txtDate, Range());
            int mon = (char)date.month;
            std::string msg = std::format("\"{0}\" does not match {1}/{2}/{3}.", txtDate, (int)date.day, mon, date.year);
            if(date.day != day || ((int)date.month != month) || date.year != year)
                Assert::Fail(toWstring(msg).c_str());
            }


    };
}
