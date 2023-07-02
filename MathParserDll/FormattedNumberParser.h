#pragma once
#include "Number.h"
#include "Scope.h"

class FormattedNumberParser
    {
    private:
        std::string _stream;
        Number parseForLocale(const std::string& str, const Range& range);
    public:
        Number parse(Scope& scope, const std::string& str, const Range& range);

    };