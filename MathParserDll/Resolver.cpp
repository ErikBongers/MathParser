#include "pch.h"
#include "Resolver.h"
#include "Function.h"
#include "Number.h"
#include "OperatorDef.h"

std::string Resolver::result = "";
void Resolver::resolve()
    {
    variables.clear();
    auto PI = Value(Number(M_PI, 0), 0, 0);
    PI.constant = true;
    variables.emplace("PI", PI);
    variables.emplace("pi", PI);
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
    else if (stmt.addExpr != nullptr)
        result = resolveNode(*stmt.addExpr);
    else
        result.onlyComment = true;
    result.text = stmt.text;
    result.comment_line = stmt.comment_line.stringValue;
    result.mute = stmt.mute;
    return result;
    }

Value Resolver::resolveNode(const Node& node)
    {
    switch (node.type)
        {
        case NodeType::BINARYOPEXPR: return resolveBinaryOp((const BinaryOpExpr&)node);
        case NodeType::STATEMENT: return resolveStatement((const Statement&)node);
        case NodeType::ASSIGNMENT: return resolveAssign((const AssignExpr&)node);
        case NodeType::PRIMARYEXPR: return resolvePrim((const PrimaryExpr&)node);
        case NodeType::POSTFIXEXPR: return resolvePostfix((const PostfixExpr&)node);
        case NodeType::CONSTEXPR: return resolveConst((const ConstExpr&)node);
        case NodeType::CALLEXPR: return resolveCall((const CallExpr&)node);
        default: return Value(ErrorId::UNKNOWN_EXPR, 0, 0);
        }
    }

Value Resolver::resolveBinaryOp(const BinaryOpExpr& addExpr)
    {
    //TODO TEST: get rid of this!
    if(OperatorDef::operators.size() == 0)
        OperatorDef::init();
    //END TODO

    Value a1 = resolveNode(*addExpr.n1);
    Value a2 = resolveNode(*addExpr.n2);
    Value result;
    
    OperatorType opType;
    switch (addExpr.op.type)
        {
        case TokenType::PLUS: opType = OperatorType::PLUS; break;
        case TokenType::MIN: opType = OperatorType::MIN; break;
        case TokenType::MULT: opType = OperatorType::MULT; break;
        case TokenType::DIV: opType = OperatorType::DIV; break;
        case TokenType::POWER: opType = OperatorType::POW; break;
        }
    OperatorDef& op = OperatorDef::get(OperatorId(ValueType::NUMBER, opType, ValueType::NUMBER, ValueType::NUMBER));
    std::vector<Value> args;
    args.push_back(a1);
    args.push_back(a2);
    result = op.call(args, a1.line, a1.pos);
    if (addExpr.error.id != ErrorId::NONE)
        result.errors.push_back(addExpr.error);
    if(!addExpr.unit.isClear())
        result = result.convertToUnit(addExpr.unit);
    return result;
    }

Value Resolver::resolveAssign(const AssignExpr& assign)
    {
    auto result = resolveNode(*assign.expr);
    if (variables.count(assign.Id.stringValue) == 0)
        {
        if (UnitDef::exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_UNIT, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
            }
        else if (FunctionDef::exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_FUNCTION, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
            }

        }
    if(variables[assign.Id.stringValue].constant)
        result.errors.push_back(Error(ErrorId::CONST_REDEF, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
    variables[assign.Id.stringValue] = result; //do not store the value with the id. The value of a variable is just the value.
    variables[assign.Id.stringValue].errors.clear(); //don't keep the errors with the variables, but only keep thew with the statement's result.
    result.id = assign.Id;
    if (assign.error.id != ErrorId::NONE)
        result.errors.push_back(assign.error);
    return result;
    }

//wrapper to parse postfixExpressions
Value Resolver::resolvePostfix(const PostfixExpr& pfix)
    {
    if (pfix.error.id != ErrorId::NONE)
        return Value(pfix.error);
    auto val = resolveNode(*pfix.expr);
    if(pfix.postfixId.isNull())
        val.unit = Unit::CLEAR();
    else if(pfix.postfixId.stringValue == "bin")
        val.numFormat = NumFormat::BIN;
    else if(pfix.postfixId.stringValue == "hex")
        val.numFormat = NumFormat::HEX;
    else if(pfix.postfixId.stringValue == "dec")
        val.numFormat = NumFormat::DEC;
    else
        val = val.convertToUnit(pfix.postfixId);
    //in case of (x.km)m, both postfixId (km) and unit (m) are filled.
    return applyUnit(pfix, val);
    }

Value Resolver::resolvePrim(const PrimaryExpr& prim)
    {
    Value val;
    if (prim.addExpr != nullptr)
        {
        auto val = resolveNode(*prim.addExpr);
        }
    else if (prim.Id.type != TokenType::NULLPTR )
        {
        auto found = variables.find(prim.Id.stringValue);
        if (found != variables.end())
            {
            val = variables[prim.Id.stringValue];
            }
        else
            return Value(ErrorId::VAR_NOT_DEF, prim.Id.line, prim.Id.pos, prim.Id.stringValue.c_str());
        }
    else if (prim.callExpr != nullptr)
        {
        auto& callExpr = *(CallExpr*)prim.callExpr;
        auto val = resolveCall(callExpr);
        }
    else
        return Value(ErrorId::UNKNOWN_EXPR, 0, 0); //TODO: this should never happen? -> allow only creation of prim with one of the above sub-types.

    return applyUnit(prim, val);
    }

Value& Resolver::applyUnit(const Node& node, Value& val)
    {
    if (!node.unit.isClear() && !val.unit.isClear())
        {
        val = val.convertToUnit(node.unit);
        }
    else if (!node.unit.isClear())
        val.unit = node.unit;
    return val;
    }

Value Resolver::resolveCall(const CallExpr& callExpr)
    {
    auto fd = FunctionDef::get(callExpr.functionName.stringValue.c_str());
    if (fd == nullptr)
        return Value(ErrorId::FUNC_NOT_DEF, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str());
    if (callExpr.error.id != ErrorId::NONE)
        return Value(callExpr.error);

    // check function arguments:
    if (!fd->isCorrectArgCount(callExpr.arguments.size()))
        return Value(ErrorId::FUNC_ARG_MIS, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str());
    std::vector<Error> errors;
    std::vector<Value> args;
    for (auto arg : callExpr.arguments)
        {
        auto argVal = resolveNode(*arg);
        errors.insert(errors.begin(), argVal.errors.begin(), argVal.errors.end());
        args.push_back(argVal);
        }
    if (hasRealErrors(errors))
        return Value(errors);

    auto val = fd->call(args, callExpr.functionName.line, callExpr.functionName.pos);
    return applyUnit(callExpr, val);
    }

Value Resolver::resolveConst(const ConstExpr& constExpr)
    {
    auto v = Value(constExpr.constNumber.numberValue, constExpr.unit, constExpr.constNumber.line, constExpr.constNumber.pos);
    v.numFormat = constExpr.constNumber.numFormat;
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
