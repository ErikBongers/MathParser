#pragma once
#include <vector>
#include "Value.h"
#include "Unit.h"
#include <string>
#include <map>

//class FunctionDefs;
struct Globals;

class FunctionDef
    {
    public:
        Globals& globals;
        std::string name;
        size_t minArgs;
        size_t maxArgs;

        FunctionDef(Globals& globals, const std::string& name, size_t minArgs, size_t maxArgs);

        bool isCorrectArgCount(size_t argCnt);
        Value call(std::vector<Value>& args, const Range& range);
    private:
        virtual Value execute(std::vector<Value>& args, const Range& range) = 0;
    };

class FunctionDefs
    {
    protected:
        std::map<std::string, FunctionDef*> functions;
        Globals& globals;

    public:
        FunctionDefs(Globals& globals) : globals(globals) { init(); }
        void init();
        bool exists(const std::string& functionName);
        void Add(FunctionDef* f) { functions.emplace(f->name, f); }
        FunctionDef* get(const std::string& name);
    };

class Statement;
class OperatorDefs;
class FunctionDefExpr;
class CustomFunction: public FunctionDef
    {
    private:
        FunctionDefExpr& functionDef;

    public:
        DateFormat dateFormat = DateFormat::UNDEFINED;

        CustomFunction(FunctionDefExpr& functionDef, Globals& globals);
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Now: public FunctionDef
    {
    public:
        Now(Globals& globals) : FunctionDef(globals, "now", 0, 0) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Inc : public FunctionDef
    {
    public:
        Inc(Globals& globals) : FunctionDef(globals, "_ inc", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Dec : public FunctionDef
    {
    public:
        Dec(Globals& globals) : FunctionDef(globals, "_ dec", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Int : public FunctionDef
    {
    public:
        Int(Globals& globals) : FunctionDef(globals, "_ int", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Abs : public FunctionDef
    {
    public:
        Abs(Globals& globals) : FunctionDef(globals, "abs", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Max : public FunctionDef
    {
    public:
        Max(Globals& globals) : FunctionDef(globals, "max", 2, 99) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Min : public FunctionDef
    {
    public:
        Min(Globals& globals) : FunctionDef(globals, "min", 2, 99) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Sin : public FunctionDef
    {
    public:
        Sin(Globals& globals) : FunctionDef(globals, "sin", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Cos : public FunctionDef
    {
    public:
        Cos(Globals& globals) : FunctionDef(globals, "cos", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Tan : public FunctionDef
    {
    public:
        Tan(Globals& globals) : FunctionDef(globals, "tan", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        ArcSin(Globals& globals) : FunctionDef(globals, "arcsin", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        ArcCos(Globals& globals) : FunctionDef(globals, "arccos", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        ArcTan(Globals& globals) : FunctionDef(globals, "arctan", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ASin : public FunctionDef
    {
    public:
        ASin(Globals& globals) : FunctionDef(globals, "asin", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ACos : public FunctionDef
    {
    public:
        ACos(Globals& globals) : FunctionDef(globals, "acos", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class ATan : public FunctionDef
    {
    public:
        ATan(Globals& globals) : FunctionDef(globals, "atan", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        Sqrt(Globals& globals) : FunctionDef(globals, "sqrt", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Round : public FunctionDef
    {
    public:
        Round(Globals& globals) : FunctionDef(globals, "round", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Floor : public FunctionDef
    {
    public:
        Floor(Globals& globals) : FunctionDef(globals, "floor", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Ceil : public FunctionDef
    {
    public:
        Ceil(Globals& globals) : FunctionDef(globals, "ceil", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };

class Trunc: public FunctionDef
    {
    public:
        Trunc(Globals& globals) : FunctionDef(globals, "trunc", 1, 1) {}
        Value execute(std::vector<Value>& args, const Range& range) override;
    };
