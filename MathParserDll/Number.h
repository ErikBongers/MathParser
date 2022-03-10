#pragma once

struct Number
    {
    double number;
    int exponent = 0;
    Number(double d, int e) : number(d), exponent(e) {}
    Number& operator++(int);
    Number& operator--(int);
    double to_double() const { return number * std::pow(10, exponent); }
    };

