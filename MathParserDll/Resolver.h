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
        Value resolveBinaryOp(const BinaryOpExpr& powerExpr);
        Value resolveAssign(const AssignExpr& assignExpr);
        Value resolvePrim(const PrimaryExpr& primExpr);
        Value& applyUnit(const Node& node, Value& val);
        Value resolvePostfix(const PostfixExpr& postfixExpr);
        Value resolveCall(const CallExpr& callExpr);
        Value resolveConst(const ConstExpr& constExpr);

    public:
        Resolver(Parser& parser) : parser(parser) {}
        void resolve();
        std::string formatError(const std::string errorMsg, ...);
        static int getResultLen() { return (int)result.size(); }
        static const std::string& getResult() { return result; }
    };

