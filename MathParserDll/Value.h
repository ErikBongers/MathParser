#pragma once
#include "Tokenizer.h"
#include "Unit.h"
#include "Error.h"
#include <sstream>
#include <iomanip>
#include <vector>

class Value
    {
    public:
        Token id = Token(TokenType::NULLPTR); //optional, in case of a variable.
        double number = 0;
        Unit unit;
        std::vector<Error> errors;

        Value() {}
        Value(double d) { this->number = d; }
        Value(ErrorId errorId, ...);
        Value(double d, const Unit u);
        Value(Token id, double d, const Unit u);
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
    };

