#pragma once
#include <functional>
#include "Range.h"
#include <set>

class Token;
class Unit
    {
    public:
        std::string id; //same ID may be used as a variable.
        Range range;

        //optional:
        Unit* subUnit1 = nullptr;
        std::string op;
        Unit* subUnit2 = nullptr;

        //Unit(const std::string& id) :id(id) {}
        Unit(const Token& idToken);
        Unit() {}
        const char* to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Unit& u);
        static Unit CLEAR();
        bool isClear() const { return id.empty(); }
    };

enum class UnitProperty { ANGLE, LENGTH, TEMP, MASS_WEIGHT, DURATION, VOLUME, UNDEFINED};

class UnitDef
    {
    private:
        double _toSI = 1; //default conversion factor

    public:
        std::string id;
        UnitProperty property = UnitProperty::UNDEFINED;

        std::function<double(double)> toSI;
        std::function<double(double)> fromSI;
        UnitDef()
            :UnitDef("", 1, UnitProperty::UNDEFINED)
            {}
        UnitDef(std::string id, double toSiFactor, UnitProperty unitClass) 
            :id(id), _toSI(toSiFactor), property(unitClass)
            {}
        void setLambda() 
            {
            //does not work in constructor.
            toSI = [this](double d) { return d * _toSI; };
            fromSI = [this](double d) { return d / _toSI; };
            }
    };

class UnitsView;
    
class UnitDefs
    {
    private:
        std::map<std::string, UnitDef> defs;
    public:
        UnitDefs() { init(); }
        void init();
        void set(UnitDef def);
        friend class UnitsView;
    };

struct Globals;

class UnitsView
    {
    private:
        Globals& globals;
        std::set<std::string> defs;
    public:
        UnitsView(const UnitsView&) = default;
        UnitsView(Globals& globals);
        void addClass(UnitProperty unitClass);
        void removeClass(UnitProperty unitClass);
        UnitDef& get(const std::string& key);
        bool exists(const std::string& key) { return defs.count(key) != 0; }
        bool isSameProperty(const Unit& u1, const Unit& u2);
        bool isUnit(const std::string& id, UnitProperty property);
        void addLongDateUnits();
        void addShortDateUnits();
        void removeShortDateUnits();
        void removeLongDateUnits();
        void setDefs(const UnitsView& uv) { this->defs = uv.defs; }
    };