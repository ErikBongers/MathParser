#pragma once
#include "ValueType.h"
#include "Value.h"

enum class OperatorType : char { NONE = '_', PLUS = '+', MIN = '-', MULT = '*', DIV = '/', POW = '^' };
typedef Value (*Operator)(Globals& globals, std::vector<Value>& args, const Range& range);

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

struct Globals;
class Resolver;
class OperatorDefs
    {
    private:
        std::map<OperatorId, Operator> operators;
    public:
        OperatorDefs(Globals& globals) : globals(globals) { init(); }
        void init();
        void Add(OperatorId id, Operator op) { operators.emplace(id, op); }
        Operator get(const OperatorId& id) { return operators[id]; }
        Globals& globals;
    };

