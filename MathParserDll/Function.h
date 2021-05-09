#pragma once
#include <vector>
#include "Value.h"
#include <string>
#include <map>

class Function
    {
    protected:
        std::vector<Value> args;
        static std::map<std::string, Function*> functions;
        static void init();

    public:
        virtual std::string getName() = 0;
        void clearArgs() { args.clear(); }
        void addArg(Value arg) { args.push_back(arg); }
        virtual Value execute() = 0;
        static bool exists(const std::string& functionName);
        static void AddFunction(Function* f) { functions.emplace(f->getName(), f); }
        static Function* get(const std::string& name);
    };

class Sin : public Function
    {
    public:
        std::string getName() override { return "sin"; }
        Value execute() override;
    };

class Cos : public Function
    {
    public:
        std::string getName() override { return "cos"; }
        Value execute() override;
    };

class Sqrt : public Function
    {
    public:
        std::string getName() override { return "sqrt"; }
        Value execute() override;
    };
