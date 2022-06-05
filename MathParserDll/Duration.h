#pragma once
#include "Error.h"

struct Number;
class Duration
    {
    public:
        static const long EmptyYears = LONG_MIN;
        static const long EmptyMonths = LONG_MIN;
        static const long EmptyDays = LONG_MIN;

        long years = EmptyYears;
        long months = EmptyMonths;
        long days = EmptyDays;

        Error error;

        void to_json(std::ostringstream& sstr) const;
        void normalize();
        double to_days() const;
        Duration(const Number& num, const Range& range);
        Duration() = default;
    };

