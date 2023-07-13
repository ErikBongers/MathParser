#include "pch.hpp"
#include "Globals.h"
#include "Scope.h"

# define M_PIl          3.141592653589793238462643383279502884L

Globals::Globals(std::vector<Source>& sources)
    : unitDefs(), functionDefs(), operatorDefs(*this), unitsView(*this), sources(sources)
    {
    auto PI = Value(Number(M_PI, 0, Range()));
    PI.constant = true;
    constants.emplace("PI", PI);
    constants.emplace("pi", PI);

    }
