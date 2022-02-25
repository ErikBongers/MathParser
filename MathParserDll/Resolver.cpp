#include "pch.h"
#include "Resolver.h"
#include "Function.h"

std::string Resolver::result = "";
void Resolver::resolve()
    {
    variables.clear();
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
    result = "{\"result\" : [" + result + "]}";
    }

Value Resolver::resolveStatement(const Statement& stmt)
    {
    Value result;
    if (stmt.assignExpr != nullptr)
        result = resolveNode(*stmt.assignExpr);
    else
        result = resolveNode(*stmt.addExpr);
    result.text = stmt.text;
    result.comment_lines = stmt.comment_lines;
    result.mute = stmt.mute;
    return result;
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
        case NodeType::POSTFIXEXPR: return resolvePostfix((const PostfixExpr&)node);
        case NodeType::CONSTEXPR: return resolveConst((const ConstExpr&)node);
        case NodeType::CALLEXPR: return resolveCall((const CallExpr&)node);
        default: return Value(ErrorId::UNKNOWN_EXPR, 0, 0);
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
    if(!addExpr.unit.isClear())
        result = result.convertToUnit(addExpr.unit);
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
    auto result = resolveNode(*assign.expr);
    variables[assign.Id.stringValue] = result; //do not store the value with the id. The value of a variable is just the value.
    result.id = assign.Id; //see comment above...
    if (assign.error.id != ErrorId::NONE)
        result.errors.push_back(assign.error);
    return result;
    }

Value Resolver::resolvePower(const PowerExpr& powerExpr)
    {
    Value base = resolveNode(*powerExpr.p1);
    Value exponent = resolveNode(*powerExpr.p2);
    auto result = base ^ exponent;
    if (powerExpr.error.id != ErrorId::NONE)
        result.errors.push_back(powerExpr.error);
    return result;
    }

//wrapper to parse postfixExpressions
Value Resolver::resolvePostfix(const PostfixExpr& pfix)
    {
    if (pfix.error.id != ErrorId::NONE)
        return Value(pfix.error);
    auto val = resolveNode(*pfix.primExpr);
    val = val.convertToUnit(pfix.postfixId);
    return val;
    }

Value Resolver::resolvePrim(const PrimaryExpr& prim)
    {
    if (prim.addExpr != nullptr)
        {
        auto val = resolveNode(*prim.addExpr);
        if (!prim.unit.isClear() && !val.unit.isClear())
            {
            val = val.convertToUnit(prim.unit);
            }
        else if(!prim.unit.isClear())
            val.unit = prim.unit;
        return val;
        }
    else if (prim.Id.type != TokenType::NULLPTR )
        {
        //if (parser.ids.count(prim.Id.stringValue) != 0)
        //    return resolveNode(*parser.ids[prim.Id.stringValue].addExpr);
        auto found = variables.find(prim.Id.stringValue);
        if (found != variables.end())
            {
            auto v = variables[prim.Id.stringValue];
            if (prim.unit.isClear())
                v.unit = Unit();
            return v;
            }
        else
            return Value(ErrorId::VAR_NOT_DEF, prim.Id.line, prim.Id.pos, prim.Id.stringValue.c_str());
        }
    else if (prim.callExpr != nullptr) //todo: create resolveCall();
        {
        auto& callExpr = *(CallExpr*)prim.callExpr;
        auto val = resolveCall(callExpr);
        if (prim.unit.isClear())
            val.unit = Unit();
        else if (!prim.unit.isClear())
            val.unit = prim.unit;
        return val;
        }
    else
        return Value(ErrorId::UNKNOWN_EXPR, 0, 0); //TODO: this should never happen? -> allow only creation of prim with one of the above sub-types.
    }

Value Resolver::resolveCall(const CallExpr& callExpr)
    {
    auto fd = FunctionDef::get(callExpr.functionName.stringValue.c_str());
    if (fd == nullptr)
        return Value(ErrorId::FUNC_NOT_DEF, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str());
    if (callExpr.error.id != ErrorId::NONE)
        return Value(callExpr.error);

    // check function arguments:
    if (callExpr.arguments.size() != fd->argsCount())
        return Value(ErrorId::FUNC_ARG_MIS, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str());
    Function f(*fd);
    std::vector<Error> errors;
    for (auto arg : callExpr.arguments)
        {
        auto argVal = resolveNode(*arg);
        errors.insert(errors.begin(), argVal.errors.begin(), argVal.errors.end());
        f.addArg(argVal);
        }
    if (errors.size() > 0)
        return Value(errors);

    auto val = f.execute(callExpr.functionName.line, callExpr.functionName.pos);
    if (callExpr.unit.isClear())
        val.unit = Unit();
    else if (!callExpr.unit.isClear())
        val.unit = callExpr.unit;
    return val;
    }

Value Resolver::resolveConst(const ConstExpr& constExpr)
    {
    auto v = Value(constExpr.constNumber.numberValue, constExpr.unit, constExpr.constNumber.line, constExpr.constNumber.pos);
    if (constExpr.unit.isClear())
        v.unit = Unit();
    else
        {
        if (UnitDef::exists(v.unit.id.stringValue) == false)
            {
            v.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, v.unit.id.line, v.unit.id.pos, v.unit.id.stringValue.c_str()));
            }
        }
    if (constExpr.error.id != ErrorId::NONE)
        v.errors.push_back(constExpr.error);
    return v;
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
