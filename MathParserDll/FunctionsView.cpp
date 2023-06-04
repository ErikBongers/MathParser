#include "pch.hpp"
#include "FunctionsView.h"
#include "Function.h"

bool FunctionsView::exists(const std::string& key) { return defs.count(key) != 0; }

