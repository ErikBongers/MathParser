#pragma once
#include "Function.h"


class Now: public FunctionDef
    {
    public:
        Now() : FunctionDef("now", 0, 0) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Inc : public FunctionDef
    {
    public:
        Inc() : FunctionDef("_ inc", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Dec : public FunctionDef
    {
    public:
        Dec() : FunctionDef("_ dec", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Abs : public FunctionDef
    {
    public:
        Abs() : FunctionDef("abs", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Max : public FunctionDef
    {
    public:
        Max() : FunctionDef("max", 1, 99) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Min : public FunctionDef
    {
    public:
        Min() : FunctionDef("min", 1, 99) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Sin : public FunctionDef
    {
    public:
        Sin() : FunctionDef("sin", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Cos : public FunctionDef
    {
    public:
        Cos() : FunctionDef("cos", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Tan : public FunctionDef
    {
    public:
        Tan() : FunctionDef("tan", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ArcSin : public FunctionDef
    {
    public:
        ArcSin() : FunctionDef("arcsin", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ArcCos : public FunctionDef
    {
    public:
        ArcCos() : FunctionDef("arccos", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ArcTan : public FunctionDef
    {
    public:
        ArcTan() : FunctionDef("arctan", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ASin : public FunctionDef
    {
    public:
        ASin() : FunctionDef("asin", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ACos : public FunctionDef
    {
    public:
        ACos() : FunctionDef("acos", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class ATan : public FunctionDef
    {
    public:
        ATan() : FunctionDef("atan", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Sqrt : public FunctionDef
    {
    public:
        Sqrt() : FunctionDef("sqrt", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Round : public FunctionDef
    {
    public:
        Round() : FunctionDef("round", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Floor : public FunctionDef
    {
    public:
        Floor() : FunctionDef("floor", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Ceil : public FunctionDef
    {
    public:
        Ceil() : FunctionDef("ceil", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Trunc: public FunctionDef
    {
    public:
        Trunc() : FunctionDef("trunc", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Factors: public FunctionDef
    {
    public:
        Factors() : FunctionDef("factors", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class DateFunc: public FunctionDef
    {
    public:
        DateFunc() : FunctionDef("date", 1, 3) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Sort: public FunctionDef
    {
    public:
        Sort() : FunctionDef("sort", 1, 99) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Reverse: public FunctionDef
    {
    public:
        Reverse() : FunctionDef("reverse", 1, 99) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Factorial: public FunctionDef
    {
    public:
        Factorial() : FunctionDef("factorial", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

class Primes: public FunctionDef
    {
    public:
        Primes() : FunctionDef("primes", 1, 1) {}
        Value execute(Scope& scope, std::vector<Value>& args, const Range& range) override;
    };

