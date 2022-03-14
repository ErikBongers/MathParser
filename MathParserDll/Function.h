#pragma once
#include <vector>
#include "Value.h"
#include <string>
#include <map>

class FunctionDef
    {
    protected:
        static std::map<std::string, FunctionDef*> functions;
        static void init();


    public:
        std::string name;
        size_t minArgs;
        size_t maxArgs;

        FunctionDef(const std::string& name, size_t minArgs, size_t maxArgs);

        bool isCorrectArgCount(size_t argCnt);
        Value call(std::vector<Value>& args, unsigned int line, unsigned int pos);
        static bool exists(const std::string& functionName);
        static void Add(FunctionDef* f) { functions.emplace(f->name, f); }
        static FunctionDef* get(const std::string& name);
    private:
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) = 0;
    };

class Inc : public FunctionDef
    {
    public:
        Inc() : FunctionDef("_ inc", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Dec : public FunctionDef
    {
    public:
        Dec() : FunctionDef("_ dec", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Int : public FunctionDef
    {
    public:
        Int() : FunctionDef("_ int", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Abs : public FunctionDef
    {
    public:
        Abs() : FunctionDef("abs", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Max : public FunctionDef
    {
    public:
        Max() : FunctionDef("max", 2, 99) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Min : public FunctionDef
    {
    public:
        Min() : FunctionDef("min", 2, 99) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sin : public FunctionDef
    {
    public:
        Sin() : FunctionDef("sin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Cos : public FunctionDef
    {
    public:
        Cos() : FunctionDef("cos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Tan : public FunctionDef
    {
    public:
        Tan() : FunctionDef("tan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        ArcSin() : FunctionDef("arcsin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        ArcCos() : FunctionDef("arccos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        ArcTan() : FunctionDef("arctan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ASin : public FunctionDef
    {
    public:
        ASin() : FunctionDef("asin", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ACos : public FunctionDef
    {
    public:
        ACos() : FunctionDef("acos", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ATan : public FunctionDef
    {
    public:
        ATan() : FunctionDef("atan", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        Sqrt() : FunctionDef("sqrt", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Round : public FunctionDef
    {
    public:
        Round() : FunctionDef("round", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Floor : public FunctionDef
    {
    public:
        Floor() : FunctionDef("floor", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Ceil : public FunctionDef
    {
    public:
        Ceil() : FunctionDef("ceil", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Trunc: public FunctionDef
    {
    public:
        Trunc() : FunctionDef("trunc", 1, 1) {}
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };
