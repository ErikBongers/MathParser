#pragma once
#include "Tokenizer.h"
#include "Unit.h"
#include "Error.h"
#include "Number.h"
#include <iomanip>
#include <vector>


class Value
    {
    public:
        Token id = Token::Null(); //optional, in case of a variable.
        Number number = {0,0};
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

        Value() : line(0), pos(0) {}
        Value(Number n, unsigned int line, unsigned int pos)
            : line(line), pos(pos)
            { this->number = n; }
        Value(ErrorId errorId, unsigned int line, unsigned int pos);
        Value(ErrorId errorId, unsigned int line, unsigned int pos, const std::string& arg1);
        Value(Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(Token id, Number n, const Unit u, unsigned int line, unsigned int pos);
        Value(const Error& error);
        Value(const std::vector<Error>& errors);

        std::string to_string();
        std::string to_string(const std::string& format);
        std::string to_json();

        Value operator+(const Value& value);
        Value operator-(const Value& value);
        Value operator*(const Value& value);
        Value operator/(const Value& value);
        Value operator^(const Value& value);
        Value convertToUnit(const Unit& to);
        double toSI() const;
        double fromSI() const;
    private:
        Value doTerm(bool adding, const Value& v);
    };

