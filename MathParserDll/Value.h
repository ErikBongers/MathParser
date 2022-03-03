#pragma once
#include "Tokenizer.h"
#include "Unit.h"
#include "Error.h"
#include <iomanip>
#include <vector>

class Value
    {
    public:
        Token id = Token::Null(); //optional, in case of a variable.
        double number = 0;
        Unit unit;
        std::vector<Error> errors;
        std::string text;
        std::string comment_line;
        bool onlyComment = false;
        bool mute = false;
        bool constant = false;
        unsigned int line;
        unsigned int pos;

        Value() : line(0), pos(0) {}
        Value(double d, unsigned int line, unsigned int pos)
            : line(line), pos(pos)
            { this->number = d; }
        Value(ErrorId errorId, unsigned int line, unsigned int pos);
        Value(ErrorId errorId, unsigned int line, unsigned int pos, const std::string& arg1);
        Value(double d, const Unit u, unsigned int line, unsigned int pos);
        Value(Token id, double d, const Unit u, unsigned int line, unsigned int pos);
        Value(const Error& error);
        Value(const std::vector<Error>& errors);

        std::string to_string();
        std::string to_string(const std::string& format);
        std::string to_json();

        Value& operator+(const Value& value);
        Value& operator-(const Value& value);
        Value& operator*(const Value& value);
        Value& operator/(const Value& value);
        Value& operator^(const Value& value);
        Value convertToUnit(const Unit& to);
        double toSI() const;
        double fromSI() const;
    private:
        Value& doTerm(bool adding, const Value& v);
    };

