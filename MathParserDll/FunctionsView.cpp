#include "pch.hpp"
#include "FunctionsView.h"
#include "Function.h"

bool FunctionsView::exists(const std::string& key) const
    { 
    return defs.count(key) != 0; 
    }

