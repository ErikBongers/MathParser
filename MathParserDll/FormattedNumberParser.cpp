#include "pch.hpp"
#include "FormattedNumberParser.h"


Number FormattedNumberParser::parse(Scope& scope, const std::string& str, const Range& range)
    {
    _stream = str;
    
    double decimalDivider = 1;
    Error error;
    int pos = 0;
    double d = 0;
    while (pos < _stream.length())
        {
        if(_stream[pos] >= '0' && _stream[pos] <= '9')
            {
            if (decimalDivider == 1)
                d = d * 10 + (_stream[pos] - '0');
            else
                {
                d += (_stream[pos] - '0') / decimalDivider;
                decimalDivider *= 10;
                }
            }
        else
            {
            if(_stream[pos] == scope.thou_char)
                {
                if(decimalDivider != 1)
                    {
                    error = Error(ErrorId::INV_NUMBER_STR, range, "thousands divider char not allowed after decimal point");
                    break;
                    }
                //note that the thou_char is currently allowed everywhere before the decimal_char !
                }
            else if(_stream[pos] == scope.decimal_char)
                {
                if(decimalDivider == 1)
                    decimalDivider = 10;
                else
                    {
                    error = Error(ErrorId::INV_NUMBER_STR, range, "decimal point encountered more than once");
                    break;
                    }
                }
            else
                {
                error = Error(ErrorId::INV_NUMBER_STR, range, "unexpected character");
                break;
                }
            }
        pos++;
        }

    auto number = Number(d,0, range);
    if(!error.isNone())
        number.errors.push_back(error);
    return number;
    }
