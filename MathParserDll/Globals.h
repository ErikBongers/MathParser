#pragma once
#include "Unit.h"
#include "Function.h"
#include "OperatorDef.h"

class FunctionDefs;
class OperatorDefs;

struct Globals
    {
    UnitDefs unitDefs;
    FunctionDefs functionDefs;
    OperatorDefs operatorDefs;

    Globals();

    };