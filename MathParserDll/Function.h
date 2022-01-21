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
        virtual std::string getName() = 0;
        virtual size_t argsCount() = 0;
        virtual Value execute(std::vector<Value>& args) = 0;
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
        Value execute() { return functionDef.execute(args); }
    };

class Max : public FunctionDef
    {
    public:
        std::string getName() override { return "max"; }
        size_t argsCount() override { return 2; }
        Value execute(std::vector<Value>& args) override;
    };

class Sin : public FunctionDef
    {
    public:
        std::string getName() override { return "sin"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class Cos : public FunctionDef
    {
    public:
        std::string getName() override { return "cos"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class Tan : public FunctionDef
    {
    public:
        std::string getName() override { return "tan"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        std::string getName() override { return "arcsin"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        std::string getName() override { return "arccos"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        std::string getName() override { return "arctan"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ASin : public FunctionDef
    {
    public:
        std::string getName() override { return "asin"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ACos : public FunctionDef
    {
    public:
        std::string getName() override { return "acos"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class ATan : public FunctionDef
    {
    public:
        std::string getName() override { return "atan"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        std::string getName() override { return "sqrt"; }
        size_t argsCount() override { return 1; }
        Value execute(std::vector<Value>& args) override;
    };
