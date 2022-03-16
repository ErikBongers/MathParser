#pragma once
#include "Tokenizer.h"
#include "Unit.h"
#include "Error.h"
#include "Number.h"
#include "ValueType.h"
#include <iomanip>
#include <vector>

class Value
    {
    public:
        Token id = Token::Null(); //optional, in case of a variable.
        ValueType type = ValueType::NONE;
        
        //possible values:
        Number number = {0,0};
        //Timepoint
        //Duration
        //Calendar

        Unit unit;
        NumFormat numFormat = NumFormat::DEC;
        std::vector<Error> errors;
        std::string text;
        std::string comment_line;
        bool onlyComment = false;
        bool mute = false;
        bool constant = false;
        unsigned int line;
        unsigned int pos;

        Value() : pos(0), line(0) {}
        Value(Number n, unsigned int line, unsigned int pos);
        Value(ErrorId errorId, unsigned int line, unsigned int pos);
        Value(ErrorId errorId, unsigned int line, unsigned int pos, const std::string& arg1);
        Value(Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(Token id, Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(const Error& error);
        Value(const std::vector<Error>& errors);

        std::string to_string();
        std::string to_string(const std::string& format);
        std::string to_json();

        Value convertToUnit(const Unit& to, UnitDefs& unitDefs);
        double toSI(UnitDefs& unitDefs) const;
        double fromSI(UnitDefs& unitDefs) const;
    };

