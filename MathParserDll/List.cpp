#include "pch.hpp"
#include "List.h"
#include <sstream>

void List::to_json(std::ostringstream& sstr) const
    {
    sstr << "[";
    bool comma = false;
    for (auto number : numberList)
        {
        if(comma)
            sstr << ",";
        number.to_json(sstr);
        comma = true;
        }

    sstr << "]";
    }

