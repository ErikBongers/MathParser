#pragma once
#include "Tokenizer.h"
#include "Error.h"
#include "Number.h"
#include "ValueType.h"
#include "Date.h"
#include "Duration.h"
#include "List.h"
#include <iomanip>
#include <vector>
#include <variant>

class Value
    {
    public:
        Token id = Token::Null(-1); //optional, in case of a variable.
        ValueType type = ValueType::NONE;
        
    private:
        std::variant<Number, Date, Duration, List> data;
    public:
        std::vector<Error> errors;
        std::string text;
        std::string comment_line;
        bool onlyComment = false;
        bool mute = false;
        bool constant = false;
        Range stmtRange;

        Value() {}
        Value(Number n);
        Value(Token id, Number n);
        Value(const Error& error);
        Value(const std::vector<Error>& errors);
        Value(const Date date);
        Value(Duration duration);
        Value(List list);
        void to_json(std::ostringstream& sstr) const;
        Number& getNumber() { return std::get<Number>(data); }
        bool isNumber() { return std::get_if<Number>(&data) != nullptr; }
        Date& getDate() { return std::get<Date>(data); }
        const Date& getDate() const { return std::get<Date>(data); } 
        Duration& getDuration() { return std::get<Duration>(data); }
        const Duration& getDuration() const { return std::get<Duration>(data); }
        List& getList() { return std::get<List>(data); }
        const List& getList() const { return std::get<List>(data); }
        void setNumber(const Number& n) { type = ValueType::NUMBER; data = n;}
        void setDate(const Date& d) { type = ValueType::TIMEPOINT; data = d;}
        void setDuration(const Duration& d) { type = ValueType::TIMEPOINT; data = d;}
        void setList(const List& l) { type = ValueType::LIST; data = l;}
    };

