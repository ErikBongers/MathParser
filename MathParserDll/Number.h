#pragma once
#include "Unit.h"
#include "Error.h"

enum class NumFormat { DEC, BIN, HEX };

struct Number
    {
    double number;
    int exponent = 0;
    Unit unit;
    NumFormat numFormat = NumFormat::DEC;
    std::vector<Error> errors;
    unsigned int line;
    unsigned int pos;

    Number(double d, int e, NumFormat numFormat = NumFormat::DEC) : number(d), exponent(e), numFormat(numFormat) {}
    Number(double d, int e, unsigned line, unsigned pos) : number(d), exponent(e), line(line), pos(pos) {}
    Number(double d, const Unit& unit, NumFormat numFormat, unsigned line, unsigned pos) : number(d), unit(unit), numFormat(numFormat), line(line), pos(pos) {}
    Number& operator++(int);
    Number& operator--(int);
    double to_double() const { return number * std::pow(10, exponent); }
    Number operator+(const Number& n2) const;
    Number operator-(const Number& n2) const;
    Number operator*(const Number& n2) const;
    Number operator/(const Number& n2) const;
    Number convertToExponent(int e) const;
    Number convertToUnit(const Unit& to, UnitDefs& unitDefs);
    double toSI(UnitDefs& unitDefs) const;
    double fromSI(UnitDefs& unitDefs) const;
    };

