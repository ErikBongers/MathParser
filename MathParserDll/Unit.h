#pragma once
#include "Tokenizer.h"

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
    };

enum class UnitClass { ANGLE, LENGTH, TEMP, WEIGHT, DATE, UNDEFINED};

struct UnitDef
    {
    public:
        static std::map<std::string, UnitDef> defs;

        std::string id;
        double toSI = 1; //conversion factor
        UnitClass property = UnitClass::UNDEFINED;
    };