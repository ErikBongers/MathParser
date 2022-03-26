#pragma once
#include "Tokenizer.h"
#include "Error.h"
#include "Number.h"
#include "ValueType.h"
#include "Date.h"
#include <iomanip>
#include <vector>
#include <variant>
class Value
    {
    public:
        Token id = Token::Null(); //optional, in case of a variable.
        ValueType type = ValueType::NONE;
        
    private:
        std::variant<Number, Date> data;
    public:
        std::vector<Error> errors;
        std::string text;
        std::string comment_line;
        bool onlyComment = false;
        bool mute = false;
        bool constant = false;

        Value() {}
        Value(Number n);
        Value(Token id, Number n);
        Value(const Error& error);
        Value(const std::vector<Error>& errors);
        Value(Date date);

        std::string to_json();
        Number& getNumber() { return std::get<Number>(data); }
        Date& getDate() { return std::get<Date>(data); }
        void setNumber(const Number& n) { type = ValueType::NUMBER; data = n;}
        void setDate(const Date& d) { type = ValueType::TIMEPOINT; data = d;}
    };

