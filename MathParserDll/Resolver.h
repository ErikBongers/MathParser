#pragma once
#include "Parser.h"
#include "Value.h"
#include "Unit.h"
#include "OperatorDef.h"

class Resolver
    {
    private:
        FunctionDefs& functionDefs;
        static std::string result;
        std::map<std::string, Value> variables;

        Value resolveNone(const NoneExpr& expr);
        Value resolveDefine(const Define& define);
        Value resolveNode(const Node& node);
        Value resolveBinaryOp(const BinaryOpExpr& expr);
        Value resolveUnaryOp(const UnaryOpExpr& expr);
        Value resolveAssign(const AssignExpr& assignExpr);
        Value resolveList(const ListExpr& listExpr);
        Value resolveIdExpr(const IdExpr& idExpr);
        Value& applyUnit(const Node& node, Value& val);
        Value resolvePostfix(const PostfixExpr& postfixExpr);
        Value resolveCall(const CallExpr& callExpr);
        Value resolveConst(const ConstExpr& constExpr);
        Value resolveDateFragment(const Value&val, const Token& fragmentId);
        Value resolveDurationFragment(const Value&val, const Token& fragmentId);
        Value resolveFunctionDef(const FunctionDefExpr& expr);

    public:
        Resolver(FunctionDefs& functionDefs, UnitDefs& unitDefs, OperatorDefs& operatorDefs, std::map<std::string, Value>& variables);
        void resolve(std::vector<Statement*>& statements);
        Value resolveStatement(const Statement& stmt);
        std::string formatError(const std::string errorMsg, ...);
        static int getResultLen() { return (int)result.size(); }
        static const std::string& getResult() { return result; }
        UnitDefs& unitDefs;
        OperatorDefs& operatorDefs;
        DateFormat dateFormat = DateFormat::UNDEFINED;
        Value& getVar(const std::string& id);
    };

