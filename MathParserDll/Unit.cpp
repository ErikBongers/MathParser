#include "pch.h"
#include "Unit.h"

std::ostream& operator<<(std::ostream& os, const Unit& u) 
    {
    os << u.to_string(); return os;
    }

const char* Unit::to_string() const
    {
    return this->id.stringValue.c_str();
    }
