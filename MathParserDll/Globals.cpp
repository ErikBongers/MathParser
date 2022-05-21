#include "pch.hpp"
#include "Globals.h"

Globals::Globals()
    : unitDefs(), functionDefs(*this), operatorDefs(*this), unitsView(*this)
    {
    }
