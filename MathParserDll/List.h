#pragma once

#include "Error.h"
#include "Number.h"

class List
    {
    public:
    Error error;
    Range range;

    std::vector<Number> numberList;
    void to_json(std::ostringstream& sstr) const;
    List(std::vector<Number> numberList) : numberList(numberList) { }
    };