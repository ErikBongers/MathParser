#include "pch.h"
#include "Unit.h"

std::ostream& operator<<(std::ostream& os, const Unit& u) 
    {
    os << u.to_string(); return os;
    }

const char* Unit::to_string() const
    {
    return this->id.stringValue.c_str();
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

        { "C", UnitDef("C", 0.000001, UnitClass::TEMP)},
        { "K", UnitDef("K", 1, UnitClass::TEMP)},
        { "F", UnitDef("F", 0.000001, UnitClass::TEMP)},

        { "L", UnitDef("L", 1, UnitClass::VOLUME)},
        { "ml", UnitDef("ml", 0.001, UnitClass::VOLUME)},
        { "gal", UnitDef("gal", 3.785411784, UnitClass::VOLUME)},
        { "pt", UnitDef("pt", 0.473176473, UnitClass::VOLUME)},

        { "kg", UnitDef("kg", 1, UnitClass::MASS_WEIGHT)},
        { "N", UnitDef("N", 1/9.80665, UnitClass::MASS_WEIGHT)}, //WEIGHT on earth, not mass!
        { "g", UnitDef("g", 0.001, UnitClass::MASS_WEIGHT)},
        { "mg", UnitDef("mg", 0.000001, UnitClass::MASS_WEIGHT)},
        { "t", UnitDef("t", 1000, UnitClass::MASS_WEIGHT)},
        { "lb", UnitDef("lb", 0.45359, UnitClass::MASS_WEIGHT)},
        { "lbs", UnitDef("lbs", 0.45359, UnitClass::MASS_WEIGHT)},
        { "oz", UnitDef("oz", 1/ 35.2739619496, UnitClass::MASS_WEIGHT)}
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
