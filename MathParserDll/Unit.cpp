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
        {"rad", {"rad", 1}},
        {"deg", {"deg", M_PI / 180}},
        {"m", {"m", 1}},
        { "cm", {"cm", 0.01} },
        { "km", {"km", 1000}},
        { "mm", {"mm", 0.001}},
        { "in", {"in", 0.0254}},
        { "thou", {"thou", 0.0254/1000}},
        { "yd", {"yd", 0.9144}},
        { "mi", {"mi", 1609.344}},
        { "micron", {"micron", 0.000001}}
    };
