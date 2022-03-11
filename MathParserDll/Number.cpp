#include "pch.h"
#include "Number.h"

Number& Number::operator++(int)
    {
    number = to_double();
    number++;
    exponent = 0;
    return *this;
    }

Number& Number::operator--(int)
    {
    number = to_double();
    number--;
    exponent = 0;
    return *this;
    }

Number Number::operator+(const Number& number2)
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.number += n2.number;
    return n1;
    }

Number Number::operator-(const Number& number2)
    {
    int maxExponent = std::max(exponent, number2.exponent);
    Number n1 = this->convertToExponent(maxExponent);
    Number n2 = number2.convertToExponent(maxExponent);
    n1.number -= n2.number;
    return n1;
    }

Number Number::operator*(const Number& n2)
    {
    return Number(number*n2.number, exponent+n2.exponent);
    }

Number Number::operator/(const Number& n2)
    {
    return Number(number/n2.number, exponent-n2.exponent);
    }

Number Number::convertToExponent(int e) const
    {
    Number result = *this;
    while (result.exponent < e)
        {
        result.exponent++;
        result.number /=10;
        }
    while (result.exponent > e)
        {
        result.exponent--;
        result.number *=10;
        }
    return result;
    }
