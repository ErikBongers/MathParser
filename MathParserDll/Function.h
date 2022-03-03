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
        bool isCorrectArgCount(size_t argCnt);
        virtual std::string getName() = 0;
        virtual size_t minArgs() = 0;
        virtual size_t maxArgs() = 0;
        virtual Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) = 0;
        static bool exists(const std::string& functionName);
        static void AddFunction(FunctionDef* f) { functions.emplace(f->getName(), f); }
        static FunctionDef* get(const std::string& name);
    };

class Function
    {
    protected:
        std::vector<Value> args;
        FunctionDef& functionDef;

    public:
        Function(FunctionDef& def) : functionDef(def) { }
        void addArg(Value arg) { args.push_back(arg); }
        Value execute(unsigned int line, unsigned int pos);
    };

class Inc : public FunctionDef
    {
    public:
        std::string getName() override { return "inc"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Dec : public FunctionDef
    {
    public:
        std::string getName() override { return "dec"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Int : public FunctionDef
    {
    public:
        std::string getName() override { return "int"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Abs : public FunctionDef
    {
    public:
        std::string getName() override { return "abs"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Max : public FunctionDef
    {
    public:
        std::string getName() override { return "max"; }
        size_t minArgs() override { return 2; }
        size_t maxArgs() override { return 99; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Min : public FunctionDef
    {
    public:
        std::string getName() override { return "min"; }
        size_t minArgs() override { return 2; }
        size_t maxArgs() override { return 99; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sin : public FunctionDef
    {
    public:
        std::string getName() override { return "sin"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Cos : public FunctionDef
    {
    public:
        std::string getName() override { return "cos"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Tan : public FunctionDef
    {
    public:
        std::string getName() override { return "tan"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        std::string getName() override { return "arcsin"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        std::string getName() override { return "arccos"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        std::string getName() override { return "arctan"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ASin : public FunctionDef
    {
    public:
        std::string getName() override { return "asin"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ACos : public FunctionDef
    {
    public:
        std::string getName() override { return "acos"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class ATan : public FunctionDef
    {
    public:
        std::string getName() override { return "atan"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        std::string getName() override { return "sqrt"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Round : public FunctionDef
    {
    public:
        std::string getName() override { return "round"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Floor : public FunctionDef
    {
    public:
        std::string getName() override { return "floor"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Ceil : public FunctionDef
    {
    public:
        std::string getName() override { return "ceil"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };

class Trunc: public FunctionDef
    {
    public:
        std::string getName() override { return "trunc"; }
        size_t minArgs() override { return 1; }
        size_t maxArgs() override { return 1; }
        Value execute(std::vector<Value>& args, unsigned int line, unsigned int pos) override;
    };
