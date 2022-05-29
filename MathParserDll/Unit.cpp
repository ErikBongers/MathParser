#include "pch.hpp"
#include "Unit.h"
#include "Globals.h"
#include "Scope.h"
#include "Token.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cassert>

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
            { "ly", UnitDef("ly", 9460730472580800, UnitClass::LENGTH)},

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
            { "oz", UnitDef("oz", 1/ 35.2739619496, UnitClass::MASS_WEIGHT)},

            { "seconds", UnitDef("seconds", 1, UnitClass::DURATION)},
            { "minutes", UnitDef("minutes", 60, UnitClass::DURATION)},
            { "hours", UnitDef("hours", 3600, UnitClass::DURATION)},
            { "days", UnitDef("days", 86400, UnitClass::DURATION)},
            { "weeks", UnitDef("weeks", 60 * 60 * 24 * 7, UnitClass::DURATION)},
            { "months", UnitDef("months", 2629746, UnitClass::DURATION)},
            { "years", UnitDef("years", 31556952, UnitClass::DURATION)},
            { "milliseconds", UnitDef("milliseconds", 1/1000, UnitClass::DURATION)},

            { "s", UnitDef("s", 1, UnitClass::DURATION)},
            { "min", UnitDef("min", 60, UnitClass::DURATION)},
            { "h", UnitDef("h", 3600, UnitClass::DURATION)},
            { "d", UnitDef("d", 86400, UnitClass::DURATION)},
            { "w", UnitDef("w", 60 * 60 * 24 * 7, UnitClass::DURATION)},
            { "mon", UnitDef("mon", 2629746, UnitClass::DURATION)},
            { "y", UnitDef("y", 31556952, UnitClass::DURATION)},
            { "ms", UnitDef("ms", 1/1000, UnitClass::DURATION)},
            };


    for (auto& [id, def] : defs)
        def.setLambda();
    defs["C"].toSI = [](double d) { return d + 273.15; };
    defs["C"].fromSI = [](double d) { return d - 273.15; };
    defs["F"].toSI = [](double d) { return (d - 32) * 5 / 9 + 273.15; };
    defs["F"].fromSI = [](double d) { return (d - 273.15) * 9 / 5 + 32; };
#ifdef  _DEBUG
    for (auto& def : defs)
        {
        assert(def.first == def.second.id);
        }
#endif //  DEBUG

    }

const char* shortDateKeys[] = { "s", "min", "h", "d", "w", "mon", "y", "ms",  };

void UnitsView::addLongDateUnits()
    {
    //TODO: addDateUnits
    }

void UnitsView::addShortDateUnits()
    {
    for(auto key: shortDateKeys)
        defs.insert(key);
    }

void UnitsView::removeShortDateUnits()
    {
    for(auto key: shortDateKeys)
        defs.erase(key);
    }

UnitsView::UnitsView(Globals& globals) 
    : globals(globals) 
    {
    addClass(UnitClass::UNDEFINED); //needed to include the empty unit.
    //TODO: make these based on settings.
    addClass(UnitClass::ANGLE);
    addClass(UnitClass::LENGTH);
    addClass(UnitClass::TEMP);
    addClass(UnitClass::VOLUME);
    addClass(UnitClass::MASS_WEIGHT);
    addClass(UnitClass::DURATION);
    removeShortDateUnits();
    }

Unit::Unit(const Token& idToken)
    {
    this->id = idToken.stringValue;
    this->range = idToken;
    }

Unit Unit::CLEAR() 
    { 
    return Unit(Token::Null(-1)); 
    }

void UnitsView::addClass(UnitClass unitClass)
    {
    for (auto& unit : globals.unitDefs.defs)
        {
        if(unit.second.property == unitClass)
            defs.insert(unit.first);
        }
    }

void UnitsView::removeClass(UnitClass unitClass)
    {
    auto it = globals.unitDefs.defs.begin();
    while (it != globals.unitDefs.defs.end()) {
        if (it->second.property == unitClass)
            defs.erase(it->second.id);
        else
            it++;
        }
    }

UnitDef& UnitsView::get(const std::string& key)
    {
    if(exists(key))
        return globals.unitDefs.defs[key];
    else
        throw std::out_of_range ("blah");
    }


bool UnitsView::isSameProperty(const Unit& u1, const Unit& u2)
    {
    if(!exists(u1.id))
        return false;
    if(!exists(u2.id))
        return false;

    return get(u1.id).property == get(u2.id).property;
    }


bool UnitsView::isUnit(const std::string& id, UnitClass property)
    {
    if(exists(id))
        return globals.unitDefs.defs[id].property == property;
    return false;
    }
