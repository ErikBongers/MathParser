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
    Number(double d, int e, NumFormat numFormat = NumFormat::DEC) : significand(d), exponent(e), numFormat(numFormat) {}
    Number(double d, int e, const Range& range) : significand(d), exponent(e), range(range) {}
    Number(double d, const Unit& unit, NumFormat numFormat, const Range& range) : significand(d), unit(unit), numFormat(numFormat), range(range) {}
    Number& operator++(int);
    Number& operator--(int);
    double to_double() const;
    Number operator+(const Number& n2) const;
    Number operator-(const Number& n2) const;
    Number operator*(const Number& n2) const;
    Number operator/(const Number& n2) const;
    Number convertToExponent(int e) const;
    Number convertToUnit(const Unit& to, UnitDefs& unitDefs);
    double toSI(UnitDefs& unitDefs) const;
    double fromSI(UnitDefs& unitDefs) const;
    void to_json(std::ostringstream& sstr) const;
    };

