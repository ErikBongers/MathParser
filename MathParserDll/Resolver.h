#pragma once
#include "Parser.h"
#include "Value.h"
#include "Unit.h"
#include "CodeBlock.h"

class Resolver
    {
    private:
        CodeBlock& codeBlock;
        Value resolveNone(const NoneExpr& expr);
        Value resolveDefine(const Define& define);
        Value resolveNode(const Node& node);
        Value resolveBinaryOp(const BinaryOpExpr& expr);
        Value resolveUnaryOp(const UnaryOpExpr& expr);
        Value resolveAssign(const AssignExpr& assignExpr);
        Value resolveList(const ListExpr& listExpr);
        Value resolveDateList(const ListExpr& listExpr);
        Value resolveDurationList(const ListExpr& listExpr);
        Value resolveIdExpr(const IdExpr& idExpr);
        Value& applyUnit(const Node& node, Value& val);
        Value resolvePostfix(const PostfixExpr& postfixExpr);
        Value resolveCall(const CallExpr& callExpr);
        Value resolveConst(const ConstExpr& constExpr);
        Value resolveDateFragment(const Value&val, const Token& fragmentId);
        Value resolveDurationFragment(const Value&val, const Token& fragmentId);
        Value resolveFunctionDef(const FunctionDefExpr& expr);

    public:
        Resolver(CodeBlock& codeBlock);
        std::string resolve();
        Value resolveBlock(const Range& range, const std::string& functionName);
        Value resolveStatement(const Statement& stmt);
        std::string formatError(const std::string errorMsg, ...);
        DateFormat dateFormat = DateFormat::UNDEFINED;
        Value& getVar(const std::string& id);
    };

