#pragma once
#include "Unit.h"
#include "OperatorDef.h"
#include "Function.h"
#include "Value.h"

class FunctionDefs;
class OperatorDefs;

struct Globals
    {
    UnitDefs unitDefs;
    FunctionDefs functionDefs;
    OperatorDefs operatorDefs;
    UnitsView unitsView;
    std::map<std::string, Value> constants;


    Globals();
    Globals(const Globals&) = default;

    };