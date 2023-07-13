#pragma once
#include "Unit.h"
#include "OperatorDef.h"
#include "Function.h"
#include "Value.h"
#include "Source.h"

class FunctionDefs;
class OperatorDefs;

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