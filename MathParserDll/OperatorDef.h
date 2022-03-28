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

class OperatorDefs;
class OperatorDef
    {
    public:
        OperatorDefs& defs;
        OperatorId id;
        OperatorDef(OperatorDefs& defs, OperatorId id) : defs(defs), id(id) {}
        Value call(std::vector<Value>& args, unsigned int line, unsigned int pos);

    private:
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) = 0;
    };

class Resolver;
class OperatorDefs
    {
    public:
        OperatorDefs(UnitDefs& unitDefs) : unitDefs(unitDefs) { init(); }
        void init();
        std::map<OperatorId, OperatorDef*> operators;
        void Add(OperatorDef* op) { operators.emplace(op->id, op); }
        OperatorDef* get(const OperatorId& id) { return operators[id]; }
        UnitDefs& unitDefs;
    };

class OpNumPlusNum : public OperatorDef
    {
    public:
        OpNumPlusNum(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::NUMBER, OperatorType::PLUS, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumMinNum : public OperatorDef
    {
    public:
        OpNumMinNum(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::NUMBER, OperatorType::MIN, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumMultNum : public OperatorDef
    {
    public:
        OpNumMultNum(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::NUMBER, OperatorType::MULT, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumDivNum : public OperatorDef
    {
    public:
        OpNumDivNum(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::NUMBER, OperatorType::DIV, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumPowNum : public OperatorDef
    {
    public:
        OpNumPowNum(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::NUMBER, OperatorType::POW, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpDateMinDate : public OperatorDef
    {
    public:
        OpDateMinDate(OperatorDefs& defs) : OperatorDef(defs, OperatorId(ValueType::TIMEPOINT, OperatorType::MIN, ValueType::TIMEPOINT, ValueType::DURATION)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };
