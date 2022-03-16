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
        UnitDefs* unitDefs;

        Value() : unitDefs(nullptr), line(0), pos(0) {}
        Value(UnitDefs* unitDefs, Number n, unsigned int line, unsigned int pos);
        Value(UnitDefs* unitDefs, ErrorId errorId, unsigned int line, unsigned int pos);
        Value(UnitDefs* unitDefs, ErrorId errorId, unsigned int line, unsigned int pos, const std::string& arg1);
        Value(UnitDefs* unitDefs, Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(UnitDefs* unitDefs, Token id, Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(UnitDefs* unitDefs, const Error& error);
        Value(UnitDefs* unitDefs, const std::vector<Error>& errors);

        std::string to_string();
        std::string to_string(const std::string& format);
        std::string to_json();

        Value convertToUnit(const Unit& to);
        double toSI() const;
        double fromSI() const;
    };

