#pragma once
#include <functional>
#include "Range.h"

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

enum class UnitClass { ANGLE, LENGTH, TEMP, MASS_WEIGHT, DURATION, VOLUME, UNDEFINED};

class UnitDef
    {
    private:
        double _toSI = 1; //default conversion factor

    public:
        std::string id;
        UnitClass property = UnitClass::UNDEFINED; //TODO rename either class or variable!

        std::function<double(double)> toSI;
        std::function<double(double)> fromSI;
        UnitDef()
            :UnitDef("", 1, UnitClass::UNDEFINED)
            {}
        UnitDef(std::string id, double toSiFactor, UnitClass unitClass) 
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
        UnitDef& get(const std::string& key);
        void set(UnitDef def);
        bool exists(const std::string& key) { return defs.count(key) != 0; }
        bool isSameProperty(const Unit& u1, const Unit& u2);
        void addDateUnits();
        void addShortDateUnits();
        friend class UnitsView;
    };

struct Globals;

class UnitsView
    {
    private:
        Globals& globals;
        std::map<std::string, UnitDef&> defs;
    public:
        UnitsView(Globals& globals) : globals(globals) {}
        void addClass(UnitClass unitClass);
        void removeClass(UnitClass unitClass);
    };