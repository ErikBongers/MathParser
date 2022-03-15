#pragma once
#include "ValueType.h"
#include "Value.h"

enum class OperatorType : char { PLUS = '+', MIN = '-', MULT = '*', DIV = '/', POW = '^' };
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

class OperatorDef
    {
    public:   
        static void init();
        static std::map<OperatorId, OperatorDef*> operators;
        OperatorId id;
        static void AddOperator(OperatorDef* op) { operators.emplace(op->id, op); }
        static OperatorDef& get(const OperatorId& id) { return *operators[id]; }
        OperatorDef(OperatorId id) : id(id) {}
        Value call(std::vector<Value>& args, unsigned int line, unsigned int pos);

    private:
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) = 0;
    };

class OpNumPlusNum : public OperatorDef
    {
    public:
        OpNumPlusNum() : OperatorDef(OperatorId(ValueType::NUMBER, OperatorType::PLUS, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumMinNum : public OperatorDef
    {
    public:
        OpNumMinNum() : OperatorDef(OperatorId(ValueType::NUMBER, OperatorType::MIN, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumMultNum : public OperatorDef
    {
    public:
        OpNumMultNum() : OperatorDef(OperatorId(ValueType::NUMBER, OperatorType::MULT, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumDivNum : public OperatorDef
    {
    public:
        OpNumDivNum() : OperatorDef(OperatorId(ValueType::NUMBER, OperatorType::DIV, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class OpNumPowNum : public OperatorDef
    {
    public:
        OpNumPowNum() : OperatorDef(OperatorId(ValueType::NUMBER, OperatorType::POW, ValueType::NUMBER, ValueType::NUMBER)) {}
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };
