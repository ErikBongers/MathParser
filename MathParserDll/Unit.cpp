#include "pch.h"
#include "Unit.h"
#include <stdexcept>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Unit& u) 
    {
    os << u.id; 
    return os;
    }

const char* Unit::to_string() const
    {
    return this->id.c_str();
    }

# define M_PIl          3.141592653589793238462643383279502884L

void UnitDefs::init()
    {
    defs = 
        {
            {"", UnitDef("", 1, UnitClass::UNDEFINED)}, //dummy unit for convenience.

            {"rad", UnitDef("rad", 1, UnitClass::ANGLE)},
            {"deg", UnitDef("deg", M_PIl / 180.0L, UnitClass::ANGLE)},

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


    for (auto& [id, def] : defs)
        def.setLambda();
    defs["C"].toSI = [](double d) { return d + 273.15; };
    defs["C"].fromSI = [](double d) { return d - 273.15; };
    defs["F"].toSI = [](double d) { return (d - 32) * 5 / 9 + 273.15; };
    defs["F"].fromSI = [](double d) { return (d - 273.15) * 9 / 5 + 32; };
    }

UnitDef& UnitDefs::get(const std::string& key)
    {
    if(exists(key))
        return defs[key];
    else
        throw std::out_of_range ("blah");
    }

void UnitDefs::set(UnitDef def)
    {
    defs.emplace(def.id, def);
    defs[def.id].setLambda();
    }

bool UnitDefs::isSameProperty(const Unit& u1, const Unit& u2)
    {
    if(!exists(u1.id))
        return false;
    if(!exists(u2.id))
        return false;

    return get(u1.id).property == get(u2.id).property;
    }

void UnitDefs::addDateUnits()
    {
    set(UnitDef("seconds", 1, UnitClass::DURATION));
    set(UnitDef("minutes", 60, UnitClass::DURATION));
    set(UnitDef("hours", 60*60, UnitClass::DURATION));
    set(UnitDef("days", 60*60*24, UnitClass::DURATION));
    set(UnitDef("weeks", 60*60*24*7, UnitClass::DURATION));
    set(UnitDef("months", 60*60*24*7*30, UnitClass::DURATION));
    set(UnitDef("years", 60*60*24*7*30*12, UnitClass::DURATION));
    }

void UnitDefs::addShortDateUnits()
    {
    set(UnitDef("s", 1, UnitClass::DURATION));
    set(UnitDef("min", 60, UnitClass::DURATION));
    set(UnitDef("h", 60*60, UnitClass::DURATION));
    set(UnitDef("d", 60*60*24, UnitClass::DURATION));
    set(UnitDef("w", 60*60*24*7, UnitClass::DURATION));
    set(UnitDef("mon", 60*60*24*7*30, UnitClass::DURATION));
    set(UnitDef("y", 60*60*24*7*30*12, UnitClass::DURATION));
    }
