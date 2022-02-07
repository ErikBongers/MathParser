#pragma once
#include "Tokenizer.h"
#include <functional>

class Unit
    {
    public:
        std::string id; //a Unit is not a tokentype by itself as the same ID may be used as a variable.

        //optional:
        Unit* subUnit1 = nullptr;
        Token op;
        Unit* subUnit2 = nullptr;

        Unit(const Token& id) :id(id.stringValue) {}
        Unit(const std::string& id) :id(id) {}
        Unit() {}
        const char* to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Unit& u);
        static Unit CLEAR() { return Unit("0"); }
        bool isClear() const { return id == "0"; }
    };

enum class UnitClass { ANGLE, LENGTH, TEMP, MASS_WEIGHT, DATE, VOLUME, UNDEFINED};

struct UnitDef
    {
    private:
        double _toSI = 1; //default conversion factor
    public:
        static std::map<std::string, UnitDef> defs;
        static void init();

        std::string id;
        UnitClass property = UnitClass::UNDEFINED;

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