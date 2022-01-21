#pragma once
#include "Parser.h"
#include "Value.h"

class Resolver
    {
    private:
        Parser& parser;
        static std::string result;
        std::map<std::string, Value> variables;

        Value resolveStatement(const Statement& stmt);
        Value resolveNode(const Node& node);
        Value resolveAdd(const AddExpr& addExpr);
        Value resolveMult(const MultExpr& multExpr);
        Value resolveAssign(const AssignExpr& assignExpr);
        Value resolvePower(const PowerExpr& powerExpr);
        Value resolvePrim(const PrimaryExpr& primExpr);
        Value resolveConst(const ConstExpr& constExpr);

    public:
        Resolver(Parser& parser) : parser(parser) {}
        void resolve();
        std::string formatError(const std::string errorMsg, ...);
        static int getResultLen() { return (int)result.size(); }
        static const std::string& getResult() { return result; }
    };

