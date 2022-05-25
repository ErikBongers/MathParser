#include "pch.h"
#include "CppUnitTest.h"
#include "../MathParserDll/api.h"
#include "../MathParserDll/Date.h"
#include "../MathParserDll/Parser.h"
#include "../MathParserDll/Function.h"
#include "../MathParserDll/Scope.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nlohmann;
namespace TestParser
{
    TEST_CLASS(TestParser)
    {
    public:
        
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
            assertDate("d = 2022,12,13", 13, 12, 2022);
            assertDate("#define dmy\n d = 13, 12, 2022", 13, 12, 2022);
            assertDate("#define mdy\n d = 12, 13, 2022", 13, 12, 2022);
            assertDate("d = 2022,12,13", 13, 12, 2022);
            assertError("d = 13, 12, 2022", "INV_DATE_VALUE");
            
            }

        TEST_METHOD(TestDefines)
            {
            assertResult("#define   date_units   short_date_units\n a=3s;", 3, "s");
            assertError("#define sdfsdf", "DEFINE_NOT_DEF");
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
            }

        TEST_METHOD(TestNumberFormats)
            {
            assertResult("123_456", 123456);
            assertResult("-123_456", -123456);
            assertResult("0b111_1011", 123, "", "", "BIN");
            assertResult("0x1E240", 123456, "", "", "HEX");
            assertResult("0b111_1011C.dec", 123, "C", "", "DEC");
            assertResult("(0b111_1011.)", 123, "", "", "BIN");
            assertResult("(0b111_1011C.).dec", 123, "", "", "DEC");
            }

        TEST_METHOD(TestExponents)
            {
            assertExponent("123E10 + 234E10", 357, 10);
            assertExponent("234E10 - 123E10", 111, 10);
            assertExponent("123E10 * 2E10", 246, 20);
            assertExponent("246E10 / 2E10", 123, 0);
            assertExponent("1E-2", 1, -2);
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

        void assertExponent(const char* stmt, double expectedNumber, int expectedExponent)
            {
            assertResult(stmt, expectedNumber, "", "", "DEC", expectedExponent);
            }

        void assertOutput(const char* stmt, double expectedResult, const std::string text, const std::string comment)
            {
            std::string msg;

            json result = assertResult(stmt, expectedResult);
            //text
            msg = std::format("\"{0}\" has incorrect comment\"{1}\", which should be \"{2}\"", stmt, (const std::string)result["text"], (const std::string)result["text"]);
            Assert::AreEqual(text, (const std::string)result["text"], toWstring(msg).c_str());
            //comment
            msg = std::format("\"{0}\" has incorrect comment\"{1}\", which should be \"{2}\"", stmt, (const std::string)result["comment"], (const std::string)result["comment"]);
            Assert::AreEqual(comment, (const std::string)result["comment"], toWstring(msg).c_str());
            }

        void assertErrors(const json& result, const char* stmt, const std::string errorId = "")
            {
            std::string msg;
            if(hasErrors(result))
                {
                if (errorId == "")
                    {
                    msg = std::format("\"{0}\" has errors: {1}", stmt, (const std::string)(result["errors"][0]["id"]));
                    Assert::Fail(toWstring(msg).c_str());
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
            json date = result["date"];
            std::string strDay = date["day"]; int d = std::stoi(strDay);
            std::string strMonth = date["month"]; int m = std::stoi(strMonth);
            std::string strYear = date["year"]; long y = std::stol(strYear);
            Assert::AreEqual(d, day);
            Assert::AreEqual(m, month);
            Assert::AreEqual(y, year);
            }
            
        json assertResult(const char* stmt, double expectedResult, const std::string expectedUnit = "", const std::string errorId = "", const std::string expectedFormat = "DEC", int expectedExponent = 0)
            {
            std::string msg;

            auto result = parseSingleResult(stmt);
            logJson(result);
            assertErrors(result, stmt, errorId);
            json number = result["number"];
            if(number["exponent"] == 0)
                {
                //value
                std::string value = number["value"];
                double d = std::stod(value);
                Assert::AreEqual(std::round(expectedResult*10000)/10000, std::round(d*10000)/10000);
                }
            else
                {
                //number
                std::string significand = number["significand"];
                double d = std::stod(significand);
                Assert::AreEqual(expectedResult, d);
                //exponent
                int e = number["exponent"];
                msg = std::format("\"{0}\" has incorrect exponent {1}, which should be {2}", stmt, (int)number["exponent"], expectedExponent);
                Assert::AreEqual(expectedExponent, (int)number["exponent"], toWstring(msg).c_str());
                }
            //unit
            msg = std::format("\"{0}\" has incorrect unit {1}, which should be {2}", stmt, (const std::string)number["unit"], expectedUnit);
            Assert::AreEqual(expectedUnit, (const std::string)number["unit"], toWstring(msg).c_str());
            //format
            msg = std::format("\"{0}\" has incorrect format {1}, which should be {2}", stmt, (const std::string)number["format"], expectedFormat);
            Assert::AreEqual(expectedFormat, (const std::string)number["format"], toWstring(msg).c_str());
            return result;
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

        bool hasErrors(json result)
            {
            auto errors = result["errors"];
            return errors.size() > 0;
            }

        bool hasError(json result, const std::string& errorId)
            {
            auto errors = result["errors"];
            for (auto& error : errors)
                {
                if(error["id"] == errorId)
                    return true;
                }
            return false;
            }

        void logJson(json result)
            {
            std::ostringstream oss;
            oss << result.dump(2) << std::endl;
            Logger::WriteMessage(oss.str().c_str());
            }

        json parseSingleResult(const char* stmt)
            {
            int resLen = parse(stmt);
            char* result = new char[resLen];
            getResult(result, resLen);
            json j = json::parse(result);
            delete[] result;
            return j["result"].back(); //back = last element, as opposed to front()
            }

        std::wstring toWstring(const std::string& s)
            {
            std::wstringstream cls;
            cls << s.c_str();
            return cls.str();
            }

        void testDateString(const char* txtDate, int day = 0, int month = 0, long year = 0)
            {
            DateParser parser;
            Date date = parser.parse(txtDate, Range());
            int mon = (char)date.month;
            std::string msg = std::format("\"{0}\" does not match {1}/{2}/{3}.", txtDate, (int)date.day, mon, date.year);
            if(date.day != day || ((int)date.month != month) || date.year != year)
                Assert::Fail(toWstring(msg).c_str());
            }


    };
}
