#include "pch.h"
#include "Unit.h"

std::ostream& operator<<(std::ostream& os, const Unit& u) 
    {
    os << u.to_string(); return os;
    }

const char* Unit::to_string() const
    {
    return this->id.c_str();
    }

std::map<std::string, UnitDef> UnitDef::defs =
    {
        {"rad", UnitDef("rad", 1, UnitClass::ANGLE)},
        {"deg", UnitDef("deg", M_PI / 180, UnitClass::ANGLE)},

        {"m", UnitDef("m", 1, UnitClass::LENGTH)},
        { "cm", UnitDef("cm", 0.01, UnitClass::LENGTH)},
        { "km", UnitDef("km", 1000, UnitClass::LENGTH)},
        { "mm", UnitDef("mm", 0.001, UnitClass::LENGTH)},
        { "in", UnitDef("in", 0.0254, UnitClass::LENGTH)},
        { "ft", UnitDef("ft", 0.3048, UnitClass::LENGTH)},
        { "thou", UnitDef("thou", 0.0254 / 1000, UnitClass::LENGTH)},
        { "yd", UnitDef("yd", 0.9144, UnitClass::LENGTH)},
        { "mi", UnitDef("mi", 1609.344, UnitClass::LENGTH)},
        { "micron", UnitDef("micron", 0.000001, UnitClass::LENGTH)},
        { "um", UnitDef("um", 0.000001, UnitClass::LENGTH)},

        { "C", UnitDef("C", 0.000001, UnitClass::LENGTH)},
        { "K", UnitDef("K", 1, UnitClass::LENGTH)},
        { "F", UnitDef("F", 0.000001, UnitClass::LENGTH)}
    };

void UnitDef::init()
    {
    for (auto& [id, def] : defs)
        def.setLambda();
    defs["C"].toSI = [](double d) { return d + 273.15; };
    defs["C"].fromSI = [](double d) { return d - 273.15; };
    defs["F"].toSI = [](double d) { return (d - 32) * 5 / 9 + 273.15; };
    defs["F"].fromSI = [](double d) { return (d - 273.15) * 9 / 5 + 32; };
    }
