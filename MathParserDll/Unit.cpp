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
            {"", UnitDef("", 1, UnitProperty::UNDEFINED)}, //dummy unit for convenience.

            {"rad", UnitDef("rad", 1, UnitProperty::ANGLE)},
            {"deg", UnitDef("deg", M_PIl / 180.0L, UnitProperty::ANGLE)},

            {"m", UnitDef("m", 1, UnitProperty::LENGTH)},
            { "cm", UnitDef("cm", 0.01, UnitProperty::LENGTH)},
            { "km", UnitDef("km", 1000, UnitProperty::LENGTH)},
            { "mm", UnitDef("mm", 0.001, UnitProperty::LENGTH)},
            { "in", UnitDef("in", 0.0254, UnitProperty::LENGTH)},
            { "ft", UnitDef("ft", 0.3048, UnitProperty::LENGTH)},
            { "thou", UnitDef("thou", 0.0254 / 1000, UnitProperty::LENGTH)},
            { "yd", UnitDef("yd", 0.9144, UnitProperty::LENGTH)},
            { "mi", UnitDef("mi", 1609.344, UnitProperty::LENGTH)},
            { "micron", UnitDef("micron", 0.000001, UnitProperty::LENGTH)},
            { "um", UnitDef("um", 0.000001, UnitProperty::LENGTH)},
            { "ly", UnitDef("ly", 9460730472580800, UnitProperty::LENGTH)},

            { "C", UnitDef("C", 0.000001, UnitProperty::TEMP)},
            { "K", UnitDef("K", 1, UnitProperty::TEMP)},
            { "F", UnitDef("F", 0.000001, UnitProperty::TEMP)},

            { "L", UnitDef("L", 1, UnitProperty::VOLUME)},
            { "ml", UnitDef("ml", 0.001, UnitProperty::VOLUME)},
            { "gal", UnitDef("gal", 3.785411784, UnitProperty::VOLUME)},
            { "pt", UnitDef("pt", 0.473176473, UnitProperty::VOLUME)},

            { "kg", UnitDef("kg", 1, UnitProperty::MASS_WEIGHT)},
            { "N", UnitDef("N", 1/9.80665, UnitProperty::MASS_WEIGHT)}, //WEIGHT on earth, not mass!
            { "g", UnitDef("g", 0.001, UnitProperty::MASS_WEIGHT)},
            { "mg", UnitDef("mg", 0.000001, UnitProperty::MASS_WEIGHT)},
            { "t", UnitDef("t", 1000, UnitProperty::MASS_WEIGHT)},
            { "lb", UnitDef("lb", 0.45359, UnitProperty::MASS_WEIGHT)},
            { "lbs", UnitDef("lbs", 0.45359, UnitProperty::MASS_WEIGHT)},
            { "oz", UnitDef("oz", 1/ 35.2739619496, UnitProperty::MASS_WEIGHT)},

            { "seconds", UnitDef("seconds", 1, UnitProperty::DURATION)},
            { "minutes", UnitDef("minutes", 60, UnitProperty::DURATION)},
            { "hours", UnitDef("hours", 3600, UnitProperty::DURATION)},
            { "days", UnitDef("days", 86400, UnitProperty::DURATION)},
            { "weeks", UnitDef("weeks", 60 * 60 * 24 * 7, UnitProperty::DURATION)},
            { "months", UnitDef("months", 2629746, UnitProperty::DURATION)},
            { "years", UnitDef("years", 31556952, UnitProperty::DURATION)},
            { "milliseconds", UnitDef("milliseconds", 1/1000, UnitProperty::DURATION)},

            { "s", UnitDef("s", 1, UnitProperty::DURATION)},
            { "min", UnitDef("min", 60, UnitProperty::DURATION)},
            { "h", UnitDef("h", 3600, UnitProperty::DURATION)},
            { "d", UnitDef("d", 86400, UnitProperty::DURATION)},
            { "w", UnitDef("w", 60 * 60 * 24 * 7, UnitProperty::DURATION)},
            { "mon", UnitDef("mon", 2629746, UnitProperty::DURATION)},
            { "y", UnitDef("y", 31556952, UnitProperty::DURATION)},
            { "ms", UnitDef("ms", 1/1000, UnitProperty::DURATION)},

            { "A", UnitDef("A", 1, UnitProperty::CURRENT)},
            { "mA", UnitDef("mA", 0.001, UnitProperty::CURRENT)},

            { "R", UnitDef("R", 1, UnitProperty::RESISTANCE)},
            { "mR", UnitDef("mR", 0.001, UnitProperty::RESISTANCE)},
            { "kR", UnitDef("kR", 1000, UnitProperty::RESISTANCE)},
            { "MR", UnitDef("MR", 1000000, UnitProperty::RESISTANCE)},

            { "V", UnitDef("V", 1, UnitProperty::VOLTAGE)},
            { "mV", UnitDef("mV", 0.001, UnitProperty::VOLTAGE)},
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
const char* longDateKeys[] = { "seconds", "minutes", "hours", "days", "weeks", "months", "years", "milliseconds", };
const char* shortDateKeys[] = { "s", "min", "h", "d", "w", "mon", "y", "ms",  };

const char* electric[] = { "A", "mA", "R", "mR", "kR", "MR", "V", "mV"};

//TODO: make these generic
void UnitsView::addLongDateUnits()
    {
    for(auto key: longDateKeys)
        defs.insert(key);
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

void UnitsView::removeLongDateUnits()
    {
    for(auto key: longDateKeys)
        defs.erase(key);
    }

void UnitsView::addElectic()
    {
    for(auto key: electric)
        defs.insert(key);
    }

void UnitsView::removeElectric()
    {
    for(auto key: electric)
        defs.erase(key);
    }

UnitsView::UnitsView(Globals& globals) 
    : globals(globals) 
    {
    addClass(UnitProperty::UNDEFINED); //needed to include the empty unit.
    addClass(UnitProperty::ANGLE);
    addClass(UnitProperty::LENGTH);
    addClass(UnitProperty::TEMP);
    addClass(UnitProperty::VOLUME);
    addClass(UnitProperty::MASS_WEIGHT);
    addClass(UnitProperty::DURATION);
    removeShortDateUnits();
    }

Unit::Unit(const std::string& id, const Range& range)
    : id(id), range(range)
    {
    }

Unit Unit::NONE() 
    { 
    return Unit("", Range());
    }

void UnitsView::addClass(UnitProperty unitClass)
    {
    for (auto& unit : globals.unitDefs.defs)
        {
        if(unit.second.property == unitClass)
            defs.insert(unit.first);
        }
    }

void UnitsView::removeClass(UnitProperty unitClass)
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


bool UnitsView::isUnit(const std::string& id, UnitProperty property)
    {
    if(exists(id))
        return globals.unitDefs.defs[id].property == property;
    return false;
    }
