#include "pch.h"
#include "CppUnitTest.h"
#include "../MathParserDll/api.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace nlohmann;
namespace TestParser
{
    TEST_CLASS(TestParser)
    {
    public:
        
        TEST_METHOD(TestPrecedence)
            {
            assertResult("a=1+2*3^4;", 163, "");
            }

        TEST_METHOD(TestUnits)
            {
            assertResult("10km+5m", 10.005, "km");
            assertResult("(10km+5m).", 10.005, "");
            assertResult("(10km+5m).m", 10005, "m");
            assertResult("10kg.g.N.lb.kg", 10, "kg");
            assertResult("max(10,20)km", 20, "km");
            assertResult("max(10m,2000m)km", 2, "km");
            assertError("a=10;a.km", "UNIT_PROP_DIFF"); //fails because unit conversion error from no unit to km. Allow this or not?
            assertError("a=10;(a).km", "UNIT_PROP_DIFF");//fails because unit conversion error from no unit to km. Allow this or not?
            assertResult("a=10;(a)km", 10, "km");
            assertError("a=10g;(a)km", "UNIT_PROP_DIFF");
            assertResult("a=10g;(a.)km", 10, "km");
            assertError("max(1kg, 4C)", "UNIT_PROP_DIFF");
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

        void assertResult(const char* stmt, double expectedResult, const std::string expectedUnit)
            {
            std::string msg;
            auto result = parseSingleResult(stmt);
            logJson(result);
            auto errors = result["errors"];
            if(errors.size() > 0)
                msg = std::format("\"{0}\" has errors: {1}", stmt, (const std::string)(errors[0]["id"]));
            Assert::IsTrue(errors.size() == 0, toWstring(msg).c_str());
            std::string value = result["value"];
            double d = std::stod(value);
            Assert::AreEqual(expectedResult, d);
            msg = std::format("\"{0}\" has incorrect unit {1}, which should be {2}", stmt, (const std::string)result["unit"], expectedUnit);
            Assert::AreEqual(expectedUnit, (const std::string)result["unit"], toWstring(msg).c_str());
            }

        void assertError(const char* stmt, const std::string errorId)
            {
            std::string msg;
            auto result = parseSingleResult(stmt);
            logJson(result);
            auto errors = result["errors"];
            for (auto& error : errors)
                {
                if(error["id"] == errorId)
                    return;
                }
            msg = std::format("\"{0}\" did not report error: {1}", stmt, errorId);
            Assert::Fail(toWstring(msg).c_str());
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
            return j["result"].back();
            }

        std::wstring toWstring(const std::string& s)
            {
            std::wstringstream cls;
            cls << s.c_str();
            return cls.str();
            }
    };
}
