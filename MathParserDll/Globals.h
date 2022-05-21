#pragma once
#include "Unit.h"
#include "OperatorDef.h"
#include "Function.h"

class FunctionDefs;
class OperatorDefs;

struct Globals
    {
    UnitDefs unitDefs;
    FunctionDefs functionDefs;
    OperatorDefs operatorDefs;
    UnitsView unitsView;

    Globals();
    Globals(const Globals&) = default;

    };