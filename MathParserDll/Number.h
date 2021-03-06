#pragma once
#include "Unit.h"
#include "Error.h"

enum class NumFormat { DEC, BIN, HEX };

struct Number
    {
    double significand=0;
    int exponent = 0;
    Unit unit;
    NumFormat numFormat = NumFormat::DEC;
    std::vector<Error> errors;
    Range range;
    Number() {}
    Number(double d, int e, const Range& range, NumFormat numFormat = NumFormat::DEC) : significand(d), exponent(e), range(range), numFormat(numFormat) {}
    Number(double d, const Unit& unit, NumFormat numFormat, const Range& range) : significand(d), unit(unit), numFormat(numFormat), range(range) {}
    Number(double d, int e, const Unit& unit, NumFormat numFormat, const Range& range) : significand(d), exponent(e), unit(unit), numFormat(numFormat), range(range) {}
    Number& operator++(int);
    Number& operator--(int);
    double to_double() const;
    Number operator+(const Number& n2) const;
    Number operator-(const Number& n2) const;
    Number operator*(const Number& n2) const;
    Number operator/(const Number& n2) const;
    Number operator%(const Number& n2) const;
    Number modulo(const Number& n2) const;
    bool operator>(const Number& n2) const;
    bool operator<(const Number& n2) const;
    Number convertToExponent(int e) const;
    Number convertToUnit(const Unit& to, UnitsView& units);
    Number toSI(UnitsView& units) const;
    Number fromSI(UnitsView& units) const;
    void to_json(std::ostringstream& sstr) const;
    };

