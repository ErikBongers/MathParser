#pragma once
#include "Parser.h"
#include "Value.h"
#include "Unit.h"
#include "OperatorDef.h"

class Resolver
    {
    private:
        Parser& parser;
        static std::string result;
        std::map<std::string, Value> variables;

        Value resolveDefine(const Define& define);
        Value resolveStatement(const Statement& stmt);
        Value resolveNode(const Node& node);
        Value resolveBinaryOp(const BinaryOpExpr& expr);
        Value resolveUnaryOp(const UnaryOpExpr& expr);
        Value resolveAssign(const AssignExpr& assignExpr);
        Value resolveList(const ListExpr& listExpr);
        Value resolvePrim(const IdExpr& primExpr);
        Value& applyUnit(const Node& node, Value& val);
        Value resolvePostfix(const PostfixExpr& postfixExpr);
        Value resolveCall(const CallExpr& callExpr);
        Value resolveConst(const ConstExpr& constExpr);
        Value resolveDateFragment(const Value&val, const Token& fragmentId);

    public:
        Resolver(Parser& parser, UnitDefs& unitDefs, OperatorDefs& operatorDefs);
        void resolve();
        std::string formatError(const std::string errorMsg, ...);
        static int getResultLen() { return (int)result.size(); }
        static const std::string& getResult() { return result; }
        UnitDefs& unitDefs;
        OperatorDefs& operatorDefs;
        Value& getVar(const std::string& id);
    };

