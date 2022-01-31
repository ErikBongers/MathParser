#include <chrono>
#include <iostream>
#include <iomanip>
#include <time.h>

namespace c = std::chrono;
using time_point = c::system_clock::time_point;
using namespace std::chrono_literals;
std::string tp_to_string(const time_point& time, const std::string& format)
    {
    std::time_t tt = c::system_clock::to_time_t(time);
    std::tm tm;
    localtime_s(&tm, &tt);
    std::stringstream ss;
    ss << std::put_time(&tm, format.c_str());
    return ss.str();
    }

int main()
    {
    time_point input = c::system_clock::now();
    std::cout << tp_to_string(input, "%Y-%m-%d %H:%M:%S") << std::endl;

    c::year_month_day ymd{ c::floor<std::chrono::days>(input) };
    ymd += c::months{ 1 };

    std::cout << "Current Year: " << static_cast<int>(ymd.year())
        << ", Month: " << static_cast<unsigned>(ymd.month())
        << ", Day: " << static_cast<unsigned>(ymd.day()) << '\n';
    auto res = c::sys_days{ ymd }; //of days is larger than possible for that month, it 'spills' to the next month: 30/02/2000 -> 02/03/2000
    std::cout << tp_to_string(res, "%Y-%m-%d %H:%M:%S") << std::endl;
    }

