#pragma once
#include "ValueType.h"
#include "Value.h"

enum class OperatorType : char { NONE = '_', PLUS = '+', MIN = '-', MULT = '*', DIV = '/', POW = '^' };
class OperatorId
    {
    public:
        ValueType type1;
        OperatorType op;
        ValueType type2;
        ValueType outType;
        bool operator<(OperatorId const& id2) const;
        std::string to_string();

        OperatorId(ValueType type1, OperatorType op, ValueType type2, ValueType outType)
            :type1(type1), op(op), type2(type2), outType(outType)
            {}
    };

struct Globals;
class OperatorDefs;
class OperatorDef
    {
    public:
        Globals& globals;
        OperatorId id;
        OperatorDef(Globals& globals, OperatorId id) : globals(globals), id(id) {}
        Value call(std::vector<Value>& args, const Range& range);

    private:
        virtual Value execute(std::vector<Value>& args, const Range& range) = 0;
    };

struct Globals;
class Resolver;
class OperatorDefs
    {
    public:
        OperatorDefs(Globals& globals) : globals(globals) { init(); }
        void init();
        std::map<OperatorId, OperatorDef*> operators;
        void Add(OperatorDef* op) { operators.emplace(op->id, op); }
        OperatorDef* get(const OperatorId& id) { return operators[id]; }
        Globals& globals;
    };

class OpNumPlusNum : public OperatorDef
    {
    public:
        OpNumPlusNum(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::NUMBER, OperatorType::PLUS, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpNumMinNum : public OperatorDef
    {
    public:
        OpNumMinNum(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::NUMBER, OperatorType::MIN, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpNumMultNum : public OperatorDef
    {
    public:
        OpNumMultNum(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::NUMBER, OperatorType::MULT, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpNumDivNum : public OperatorDef
    {
    public:
        OpNumDivNum(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::NUMBER, OperatorType::DIV, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpNumPowNum : public OperatorDef
    {
    public:
        OpNumPowNum(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::NUMBER, OperatorType::POW, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpDateMinDate : public OperatorDef
    {
    public:
        OpDateMinDate(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::TIMEPOINT, OperatorType::MIN, ValueType::TIMEPOINT, ValueType::DURATION)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };

class OpDatePlusDur: public OperatorDef
    {
    public:
        OpDatePlusDur(Globals& globals) : OperatorDef(globals, OperatorId(ValueType::TIMEPOINT, OperatorType::PLUS, ValueType::DURATION, ValueType::TIMEPOINT)) {}
        virtual Value execute(std::vector<Value>& args, const Range& range) override;
    };
