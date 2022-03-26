#pragma once
#include <vector>
#include "Value.h"
#include "Unit.h"
#include <string>
#include <map>

class FunctionDefs;
    
class FunctionDef
    {
    public:
        FunctionDefs& functionDefs;
        std::string name;
        size_t minArgs;
        size_t maxArgs;

        FunctionDef(FunctionDefs& functionDefs, const std::string& name, size_t minArgs, size_t maxArgs);

        bool isCorrectArgCount(size_t argCnt);
        Value call(std::vector<Value>& args, unsigned int line, unsigned int pos);
    private:
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) = 0;
    };

class FunctionDefs
    {
    protected:
        std::map<std::string, FunctionDef*> functions;

    public:
        FunctionDefs(UnitDefs& unitDefs) : unitDefs(unitDefs) { init(); }
        void init();
        bool exists(const std::string& functionName);
        void Add(FunctionDef* f) { functions.emplace(f->name, f); }
        FunctionDef* get(const std::string& name);
        UnitDefs& unitDefs;
    };

class Now: public FunctionDef
    {
    public:
        Now(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "now", 0, 0) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Inc : public FunctionDef
    {
    public:
        Inc(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "_ inc", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Dec : public FunctionDef
    {
    public:
        Dec(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "_ dec", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Int : public FunctionDef
    {
    public:
        Int(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "_ int", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Abs : public FunctionDef
    {
    public:
        Abs(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "abs", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Max : public FunctionDef
    {
    public:
        Max(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "max", 2, 99) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Min : public FunctionDef
    {
    public:
        Min(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "min", 2, 99) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sin : public FunctionDef
    {
    public:
        Sin(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "sin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Cos : public FunctionDef
    {
    public:
        Cos(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "cos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Tan : public FunctionDef
    {
    public:
        Tan(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "tan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        ArcSin(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "arcsin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        ArcCos(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "arccos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        ArcTan(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "arctan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ASin : public FunctionDef
    {
    public:
        ASin(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "asin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ACos : public FunctionDef
    {
    public:
        ACos(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "acos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ATan : public FunctionDef
    {
    public:
        ATan(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "atan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        Sqrt(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "sqrt", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Round : public FunctionDef
    {
    public:
        Round(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "round", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Floor : public FunctionDef
    {
    public:
        Floor(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "floor", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Ceil : public FunctionDef
    {
    public:
        Ceil(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "ceil", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Trunc: public FunctionDef
    {
    public:
        Trunc(FunctionDefs& functionDefs) : FunctionDef(functionDefs, "trunc", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };
