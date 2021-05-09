#include "pch.h"
#include "Resolver.h"
#include "Function.h"

std::string Resolver::result = "";
void Resolver::resolve()
    {
    std::vector<std::string> jsonRes;
    parser.parse();
    for (auto& stmt : parser.statements)
        {
        auto result = resolveStatement(*stmt);
        jsonRes.push_back(result.to_json());
        }
    result = "";
    for (auto& s : jsonRes)
        result += s + ",";
    if(result.size() > 0)
        result.resize(result.size()-1); //remove last comma;
    }

Value Resolver::resolveStatement(const Statement& stmt)
    {
    if (stmt.assignExpr != nullptr)
        return resolveNode(*stmt.assignExpr);
    else
        return resolveNode(*stmt.addExpr);
    }

Value Resolver::resolveNode(const Node& node)
    {
    switch (node.type)
        {
        case NodeType::ADDEXPR : return resolveAdd((const AddExpr&)node);
        case NodeType::MULTEXPR: return resolveMult((const MultExpr&)node);
        case NodeType::STATEMENT: return resolveStatement((const Statement&)node);
        case NodeType::ASSIGNMENT: return resolveAssign((const AssignExpr&)node);
        case NodeType::POWEREXPR: return resolvePower((const PowerExpr&)node);
        case NodeType::PRIMARYEXPR: return resolvePrim((const PrimaryExpr&)node);
        case NodeType::CONSTEXPR: return resolveConst((const ConstExpr&)node);
        default: return Value(ErrorId::UNKNOWN_EXPR, nullptr);
        }
    }

Value Resolver::resolveAdd(const AddExpr& addExpr)
    {
    Value a1 = resolveNode(*addExpr.a1);
    Value a2 = resolveNode(*addExpr.a2);
    Value result;
    if (addExpr.op.type == TokenType::PLUS)
        result = a1 + a2;
    else
        result = a1 - a2;
    if (addExpr.error.id != ErrorId::NONE)
        result.errors.push_back(addExpr.error);
    return result;
    }

Value Resolver::resolveMult(const MultExpr& multExpr)
    {
    Value m1 = resolveNode(*multExpr.m1);
    Value m2 = resolveNode(*multExpr.m2);
    Value result;
    if (multExpr.op.type == TokenType::MULT)
        result = m1 * m2;
    else
        result = m1 / m2;
    if (multExpr.error.id != ErrorId::NONE)
        result.errors.push_back(multExpr.error);
    return result;
    }

Value Resolver::resolveAssign(const AssignExpr& assign)
    {
    auto result = resolveNode(*assign.addExpr);
    result.id = assign.Id;
    if (assign.error.id != ErrorId::NONE)
        result.errors.push_back(assign.error);
    return result;
    }

Value Resolver::resolvePower(const PowerExpr& powerExpr)
    {
    Value prim = resolveNode(*powerExpr.p2);
    if (powerExpr.p1 == nullptr)
        return prim;
    Value power = resolveNode(*powerExpr.p1);
    auto result = power ^ prim;
    if (powerExpr.error.id != ErrorId::NONE)
        result.errors.push_back(powerExpr.error);
    return result;
    }

Value Resolver::resolvePrim(const PrimaryExpr& prim)
    {
    if (prim.addExpr != nullptr)
        return resolveNode(*prim.addExpr);
    else if (prim.Id.type != TokenType::NULLPTR )
        {
        if (parser.ids.count(prim.Id.stringValue) != 0)
            return resolveNode(*parser.ids[prim.Id.stringValue].addExpr);
        else
            return Value(ErrorId::VAR_NOT_DEF, prim.Id.stringValue.c_str());
        }
    else if (prim.callExpr != nullptr)
        {
        auto f = Function::get(((CallExpr*)prim.callExpr)->functionName.stringValue.c_str());
        if (f == nullptr)
            return Value(ErrorId::FUNC_NOT_DEF, prim.Id.stringValue.c_str());
        if (((CallExpr*)prim.callExpr)->argument == nullptr)
            return Value(ErrorId::FUNC_ARG_MIS, prim.Id.stringValue.c_str());
        f->clearArgs();
        f->addArg(resolveNode(*((CallExpr*)prim.callExpr)->argument));
        return f->execute();
        }
    else
        return Value(ErrorId::UNKNOWN_EXPR);
    }

Value Resolver::resolveConst(const ConstExpr& constExpr)
    {
    return Value(constExpr.constNumber.numberValue, constExpr.unit);
    }

std::string Resolver::formatError(const std::string errorMsg, ...)
    {
    char buffer[200];
    va_list args;
    va_start(args, errorMsg);
    snprintf(buffer, 200, errorMsg.c_str(), args);
    va_end(args);
    return buffer;
    }
