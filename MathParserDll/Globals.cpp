#include "pch.hpp"
#include "Globals.h"
#include "Scope.h"

Globals::Globals()
    : unitDefs(), functionDefs(*this), operatorDefs(*this), unitsView(*this)
    {
    }
