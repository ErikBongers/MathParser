#pragma once
#include "Unit.h"
#include "OperatorDef.h"
#include "Function.h"
#include "Value.h"

class FunctionDefs;
class OperatorDefs;

struct Source
    {
    char index = -1;
    std::string name;
    std::string text;
    };

struct Globals
    {
    UnitDefs unitDefs;
    FunctionDefs functionDefs;
    OperatorDefs operatorDefs;
    UnitsView unitsView;
    std::map<std::string, Value> constants;
    std::vector<Source>& sources;

    Globals(std::vector<Source>& sources);
    Globals(const Globals&) = default;

    };