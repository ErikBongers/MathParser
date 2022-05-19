#include "pch.hpp"
#include "Resolver.h"
#include "Function.h"
#include "Number.h"
#include "OperatorDef.h"
#include "trim.h"
#include "tools.h"
#include <sstream>

Resolver::Resolver(Globals& globals, std::map<std::string, Value>& variables) 
    : globals(globals)
    {
    this->variables = variables;
    }

std::string Resolver::resolve(std::vector<Statement*>& statements)
    {
    auto PI = Value(Number(M_PI, 0, Range()));
    PI.constant = true;
    variables.emplace("PI", PI);
    variables.emplace("pi", PI);
    std::vector<std::string> jsonRes;
    std::ostringstream sstr;
    for (auto& stmt : statements)
        {
        auto result = resolveStatement(*stmt);
        if (stmt->node != nullptr)
            {
            if(stmt->node->type != NodeType::DEFINE || result.errors.size() != 0)
                {
                result.to_json(sstr);
                sstr << ",";
                }
            }
        else
            {
            result.to_json(sstr);
            sstr << ",";
            }
        }
    std::string result = sstr.str();
    if(result.size() > 0)
        result.resize(result.size()-1); //remove last comma;
    result = "{\"result\" : [" + result + "]}";
    return result;
    }

Value Resolver::resolveDefine(const Define& define)
    {
    Value result;
    for(auto& t: define.defs) 
        {
        switch(hash(t.stringValue.c_str()))
            {
            case hash("date_units"):
                globals.unitDefs.addDateUnits(); 
                break;
            case hash("short_date_units"):
                globals.unitDefs.addShortDateUnits(); 
                break;
            case hash("ymd"):
                dateFormat = DateFormat::YMD;
                break;
            case hash("dmy"):
                dateFormat = DateFormat::DMY;
                break;
            case hash("mdy"):
                dateFormat = DateFormat::MDY;
                break;
            default:
                result.errors.push_back(Error(ErrorId::DEFINE_NOT_DEF, define.range(), t.stringValue));
            }
        }
    return result;
    }

Value Resolver::resolveStatement(const Statement& stmt)
    {
    Value result;
    if (stmt.node != nullptr)
        result = resolveNode(*stmt.node);
    else
        result.onlyComment = true;
    result.text = stmt.text;
    result.comment_line = stmt.comment_line.stringValue;
    result.mute = stmt.mute;
    if (stmt.error.id != ErrorId::NONE)
        result.errors.push_back(stmt.error);
    result.stmtRange = stmt.range();
    return result;
    }

Value Resolver::resolveNode(const Node& node)
    {
    switch (node.type)
        {
        case NodeType::BINARYOPEXPR: return resolveBinaryOp((const BinaryOpExpr&)node);
        case NodeType::UNARYOPEXPR: return resolveUnaryOp((const UnaryOpExpr&)node);
        case NodeType::STATEMENT: return resolveStatement((const Statement&)node);
        case NodeType::ASSIGNMENT: return resolveAssign((const AssignExpr&)node);
        case NodeType::LIST: return resolveList((const ListExpr&)node);
        case NodeType::IDEXPR: return resolveIdExpr((const IdExpr&)node);
        case NodeType::POSTFIXEXPR: return resolvePostfix((const PostfixExpr&)node);
        case NodeType::CONSTEXPR: return resolveConst((const ConstExpr&)node);
        case NodeType::CALLEXPR: return resolveCall((const CallExpr&)node);
        case NodeType::DEFINE: return resolveDefine((const Define&)node);
        case NodeType::NONE: return resolveNone((const NoneExpr&)node);
        case NodeType::FUNCTIONDEF: return resolveFunctionDef((const FunctionDefExpr&)node);
        default: return Value(Error(ErrorId::UNKNOWN_EXPR, node.range()));
        }
    }

Value Resolver::resolveFunctionDef(const FunctionDefExpr& expr)
    {
    CustomFunction* f = dynamic_cast<CustomFunction*>(globals.functionDefs.get(expr.id.stringValue));
    f->dateFormat = dateFormat;
    return Value(); //none
    }

Value Resolver::resolveNone(const NoneExpr& expr)
    {
    Value v;
    if(expr.error.id != ErrorId::NONE)
        v.errors.push_back(expr.error);
    return v;
    }

Value Resolver::resolveBinaryOp(const BinaryOpExpr& expr)
    {
    Value a1 = resolveNode(*expr.n1);
    Value a2 = resolveNode(*expr.n2);
    Value result;

    result.errors.insert(result.errors.begin(), a1.errors.begin(), a1.errors.end());
    result.errors.insert(result.errors.begin(), a2.errors.begin(), a2.errors.end());
    if(hasRealErrors(result.errors))
        return result;
    OperatorType opType = OperatorType::NONE;
    switch (expr.op.type)
        {
        case TokenType::PLUS: opType = OperatorType::PLUS; break;
        case TokenType::MIN: opType = OperatorType::MIN; break;
        case TokenType::MULT: opType = OperatorType::MULT; break;
        case TokenType::DIV: opType = OperatorType::DIV; break;
        case TokenType::POWER: opType = OperatorType::POW; break;
        default: break;
        }
    OperatorDef* op = globals.operatorDefs.get(OperatorId(a1.type, opType, a2.type, a1.type));
    if (op == nullptr)
        {
        result.errors.push_back(Error(ErrorId::NO_OP, Range(expr.op), expr.op.stringValue, to_string(a1.type), to_string(a2.type)));
        return result;
        }
    std::vector<Value> args;
    args.push_back(a1);
    args.push_back(a2);
    result = op->call(args, expr.op);
    if (expr.error.id != ErrorId::NONE)
        result.errors.push_back(expr.error);
    if(result.type ==ValueType::NUMBER && !expr.unit.isClear())
        result.setNumber(result.getNumber().convertToUnit(expr.unit, globals.unitDefs));
    return result;
    }

Value Resolver::resolveUnaryOp(const UnaryOpExpr& expr)
    {
    Value a1 = resolveNode(*expr.n);
    if (expr.op.type == TokenType::MIN)
        {
        if (a1.type == ValueType::NUMBER)
            {
            a1.getNumber().significand = -a1.getNumber().significand;
            }
        }
    return a1;
    }

Value Resolver::resolveAssign(const AssignExpr& assign)
    {
    auto result = resolveNode(*assign.expr);
    if (variables.count(assign.Id.stringValue) == 0)
        {
        if (globals.unitDefs.exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_UNIT, Range(assign.Id), assign.Id.stringValue));
            }
        else if (globals.functionDefs.exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_FUNCTION, Range(assign.Id), assign.Id.stringValue));
            }
        variables.emplace(assign.Id.stringValue, Value()); //create var, regardless of errors.
        }
    auto& var = getVar(assign.Id.stringValue);
    if(var.constant)
        result.errors.push_back(Error(ErrorId::CONST_REDEF, Range(assign.Id), assign.Id.stringValue));
    var = result; //do not store the value with the id. The value of a variable is just the value.
    var.errors.clear(); //don't keep the errors with the variables, but only keep thew with the statement's result.
    result.id = assign.Id;
    if (assign.error.id != ErrorId::NONE)
        result.errors.push_back(assign.error);
    return result;
    }

Value Resolver::resolveList(const ListExpr& listExpr)
    {
    //just a minimal implementation for now...
    if(listExpr.list.size() != 3)
        return Value();
    int iDay = 0, iMonth = 0, iYear = 0;

    switch (dateFormat)
        {
        using enum DateFormat;
        case DMY:
            iDay = 0; iMonth = 1; iYear = 2; break;
        case MDY:
            iMonth = 0; iDay = 1; iYear = 2; break;
        case YMD:
        case UNDEFINED: //fall through
            iYear = 0; iMonth = 1; iDay = 2; break;
        }
    auto day = resolveNode(*listExpr.list[iDay]);
    auto month = resolveNode(*listExpr.list[iMonth]);
    auto year = resolveNode(*listExpr.list[iYear]);
    Date date;
    if(day.type != ValueType::NUMBER) 
        return Value(Error(ErrorId::INV_DATE_VALUE, listExpr.list[iDay]->range(), "?", "day"));
    if(month.type != ValueType::NUMBER) 
        return Value(Error(ErrorId::INV_DATE_VALUE, listExpr.list[iMonth]->range(), "?", "month"));
    if(year.type != ValueType::NUMBER) 
        return Value(Error(ErrorId::INV_DATE_VALUE, listExpr.list[iYear]->range(), "?", "year"));

    if(day.getNumber().to_double() < 1 || day.getNumber().to_double() > 31)// don't check date.day as it might have been truncated!
        return Value(Error(ErrorId::INV_DATE_VALUE, listExpr.list[iDay]->range(), std::to_string(day.getNumber().to_double()), "day"));
    if(month.getNumber().to_double() < 1 || month.getNumber().to_double() > 12)
        return Value(Error(ErrorId::INV_DATE_VALUE, listExpr.list[iMonth]->range(), std::to_string(month.getNumber().to_double()), "month"));

    date.day = (char)day.getNumber().to_double();
    date.month = (Month)month.getNumber().to_double();
    date.year = (long)year.getNumber().to_double();

    date.errors.insert(date.errors.end(), day.getNumber().errors.begin(), day.getNumber().errors.end());
    date.errors.insert(date.errors.end(), month.getNumber().errors.begin(), month.getNumber().errors.end());
    date.errors.insert(date.errors.end(), year.getNumber().errors.begin(), year.getNumber().errors.end());
    return Value(date);
    }

//wrapper to parse postfixExpressions
Value Resolver::resolvePostfix(const PostfixExpr& pfix)
    {
    if (pfix.error.id != ErrorId::NONE)
        return Value(pfix.error);
    auto val = resolveNode(*pfix.expr);
    if(pfix.postfixId.isNull() && val.type == ValueType::NUMBER)
        val.getNumber().unit = Unit::CLEAR();
    else 
        switch(hash(pfix.postfixId.stringValue.c_str()))
            {
            case hash("bin"):
                val.getNumber().numFormat = NumFormat::BIN; break;
            case hash("hex"):
                val.getNumber().numFormat = NumFormat::HEX; break;
            case hash("dec"):
                val.getNumber().numFormat = NumFormat::DEC; break;
            case hash("day"):
            case hash("month"):
            case hash("year"):
                val = resolveDateFragment(val, pfix.postfixId);
                break;
            case hash("days"):
            case hash("months"):
            case hash("years"):
                if(val.type == ValueType::DURATION)
                    {
                    val = resolveDurationFragment(val, pfix.postfixId);
                    break;
                    }
                [[fallthrough]];
            default:
                if(val.type == ValueType::NUMBER)
                    val.getNumber() = val.getNumber().convertToUnit(pfix.postfixId, globals.unitDefs);
                else
                    ; //TODO: error: unknown postfix
                break;
            }
    //in case of (x.km)m, both postfixId (km) and unit (m) are filled.
    return applyUnit(pfix, val);
    }

Value Resolver::resolveIdExpr(const IdExpr& idExpr)
    {
    Value val;
    auto found = variables.find(idExpr.Id.stringValue);
    if (found != variables.end())
        {
        val = getVar(idExpr.Id.stringValue);
        }
    else
        return Value(Error(ErrorId::VAR_NOT_DEF, Range(idExpr.Id), idExpr.Id.stringValue.c_str()));

    return applyUnit(idExpr, val);
    }

Value& Resolver::applyUnit(const Node& node, Value& val)
    {
    if(val.type != ValueType::NUMBER)
        return val;
    if (!node.unit.isClear() && !val.getNumber().unit.isClear())
        {
        val.getNumber ()= val.getNumber().convertToUnit(node.unit, globals.unitDefs);
        }
    else if (!node.unit.isClear())
        {
        if (globals.unitDefs.exists(node.unit.id))
            val.getNumber().unit = node.unit;
        else
            val.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, node.unit.range, node.unit.id.c_str()));
        }
    return val;
    }

Value Resolver::resolveCall(const CallExpr& callExpr)
    {
    auto fd = globals.functionDefs.get(callExpr.functionName.stringValue.c_str());
    if (fd == nullptr)
        return Value(Error(ErrorId::FUNC_NOT_DEF, Range(callExpr.functionName), callExpr.functionName.stringValue.c_str()));
    if (callExpr.error.id != ErrorId::NONE)
        return Value(callExpr.error);

    // check function arguments:
    if (!fd->isCorrectArgCount(callExpr.arguments.size()))
        return Value(Error(ErrorId::FUNC_ARG_MIS, Range(callExpr.functionName), callExpr.functionName.stringValue.c_str()));
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

    auto val = fd->call(args, callExpr.functionName);
    return applyUnit(callExpr, val);
    }

Value Resolver::resolveConst(const ConstExpr& constExpr)
    {
    if(constExpr.type == ValueType::NUMBER)
        {
        auto v = Value(constExpr.value.numberValue);
        v.getNumber().unit = constExpr.unit;
        if (constExpr.unit.isClear())
            v.getNumber().unit = Unit();
        else
            {
            if (globals.unitDefs.exists(v.getNumber().unit.id) == false)
                {
                v.getNumber().errors.push_back(Error(ErrorId::UNIT_NOT_DEF, v.getNumber().unit.range, v.getNumber().unit.id.c_str()));
                }
            }
        if (constExpr.error.id != ErrorId::NONE)
            v.errors.push_back(constExpr.error);
        return v;
        }
    else
        {
        DateParser parser;
        parser.dateFormat = this->dateFormat;
        return Value(parser.parse(constExpr.value.stringValue, constExpr.range()));
        }
    }

Value Resolver::resolveDateFragment(const Value& val, const Token& fragmentId)
    {
    Value newValue;
    if(val.type != ValueType::TIMEPOINT)
        return Value(Error(ErrorId::DATE_FRAG_NO_DATE, fragmentId, fragmentId.stringValue));
    const Date date = val.getDate();
    switch (hash(fragmentId.stringValue.c_str()))
        {
        case hash("day"):
            newValue = Value(Number(date.day, 0)); break;
        case hash("month"):
            newValue = Value(Number((double)date.month, 0)); break;
        case hash("year"):
            newValue = Value(Number(date.year, 0)); break;
        default:
            return Value(Error(ErrorId::DATE_INV_FRAG, fragmentId, fragmentId.stringValue));
        }
    return newValue;
    }

Value Resolver::resolveDurationFragment(const Value& val, const Token& fragmentId)
    {
    Value newValue;
    if(val.type != ValueType::DURATION)
        return Value(Error(ErrorId::DATE_FRAG_NO_DURATION, fragmentId, fragmentId.stringValue));
    const auto dur = val.getDuration();
    switch (hash(fragmentId.stringValue.c_str()))
        {
        case hash("days"):
            newValue = Value(Number(dur.days, 0)); break;
        case hash("months"):
            newValue = Value(Number((double)dur.months, 0)); break;
        case hash("years"):
            newValue = Value(Number(dur.years, 0)); break;
        default:
            return Value(Error(ErrorId::DUR_INV_FRAG, fragmentId, fragmentId.stringValue));
        }
    return newValue;
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

Value& Resolver::getVar(const std::string& id)
    {
    return variables.find(id)->second;
    }
