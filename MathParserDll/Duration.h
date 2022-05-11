#pragma once

class Duration
    {
    public:
        static const long EmptyYears = LONG_MIN;
        static const long EmptyMonths = LONG_MIN;
        static const long EmptyDays = LONG_MIN;

        long years = EmptyYears;
        long months = EmptyMonths;
        long days = EmptyDays;

        void to_json(std::ostringstream& sstr) const;
        void normalize();
    };

