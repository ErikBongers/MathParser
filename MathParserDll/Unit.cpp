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
        {"rad", {"rad", 1, UnitClass::ANGLE}},
        {"deg", {"deg", M_PI / 180, UnitClass::ANGLE}},

        {"m", {"m", 1, UnitClass::LENGTH}},
        { "cm", {"cm", 0.01, UnitClass::LENGTH}},
        { "km", {"km", 1000, UnitClass::LENGTH}},
        { "mm", {"mm", 0.001, UnitClass::LENGTH}},
        { "in", {"in", 0.0254, UnitClass::LENGTH}},
        { "thou", {"thou", 0.0254 / 1000, UnitClass::LENGTH}},
        { "yd", {"yd", 0.9144, UnitClass::LENGTH}},
        { "mi", {"mi", 1609.344, UnitClass::LENGTH}},
        { "micron", {"micron", 0.000001, UnitClass::LENGTH}},
        { "um", {"um", 0.000001, UnitClass::LENGTH}}
    };
