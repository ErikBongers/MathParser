#include "pch.hpp"
#include "Resolver.h"
#include "Function.h"
#include "Number.h"
#include "OperatorDef.h"
#include "trim.h"
#include "tools.h"
#include "Globals.h"
#include "Scope.h"
#include "NodeFactory.h"
#include "FormattedNumberParser.h"
#include "FormattedDateParser.h"

Resolver::Resolver(CodeBlock& codeBlock) 
    : codeBlock(codeBlock)
    {
    }

std::string Resolver::resolve()
    {
    resolve(output);

    std::string result = output.str();
    if(result.size() > 0)
        result.resize(result.size()-1); //remove last comma;
    result = "{\"result\" : [" + result + "]}";
    
    return result;
    }

void Resolver::resolve(std::ostringstream& sstr)
    {
    for (auto& stmt : codeBlock.statements)
        {
        auto result = resolveStatement(*stmt);
        if (stmt->node != nullptr)
            {
            if(stmt->node->type != NodeType::DEFINE || result.errors.size() != 0)
                {
                result.to_json(sstr, *codeBlock.scope);
                sstr << ",";
                }
            }
        else
            {
            result.to_json(sstr, *codeBlock.scope);
            sstr << ",";
            }
        }
    }

Value Resolver::resolveFunctionBlock(const Range& range, const std::string& functionName)
    {
    Value result;
    std::vector<Error> errors;
    for(auto stmt : codeBlock.statements)
        {
        result = resolveStatement(*stmt);
        errors.insert(errors.end(), result.errors.begin(), result.errors.end());
        }
    if(!errors.empty())
        {
        auto error = Error(ErrorId::FUNC_FAILED, range, functionName);
        error.stackTrace = std::move(errors);
        result.errors.clear();
        result.errors.push_back(error);
        }
    return result;
    }

 FunctionType toFunctionType(uint32_t funcHash)
    {
    switch(funcHash)
        {
        using enum FunctionType;
        case hash("trig"): return TRIG;
        case hash("arithm"): return ARITHM;
        case hash("date"): return DATE;
        case hash("all"): return ALL;
        default: throw "blah";
        }
    }

Value Resolver::resolveDefine(const Define& define)
    {
    Value result;
    if(!define.undef)
        {
        for(auto& t: define.defs) 
            {
            switch(hash(codeBlock.scope->getText(t.range).c_str()))
                {
                case hash("date_units"):
                    codeBlock.scope->units.addLongDateUnits(); 
                    break;
                case hash("short_date_units"):
                    codeBlock.scope->units.addShortDateUnits(); 
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
                case hash("trig"):
                case hash("arithm"):
                case hash("date"):
                case hash("all"):
                    codeBlock.scope->functions.addFunctions(toFunctionType(hash(codeBlock.scope->getText(t.range).c_str())));
                    break;
                case hash("electric"):
                    codeBlock.scope->units.addElectic();
                    break;
                case hash("strict"):
                    codeBlock.scope->strict = true;
                    break;
                case hash("decimal_dot"):
                case hash("dec_dot"):
                case hash("dot"):
                    codeBlock.scope->decimal_char = '.';
                    codeBlock.scope->thou_char = ',';
                    break;
                case hash("decimal_comma"):
                case hash("dec_comma"):
                case hash("comma"):
                    codeBlock.scope->decimal_char = ',';
                    codeBlock.scope->thou_char = '.';
                    break;
                default:
                    result.errors.push_back(Error(ErrorId::DEFINE_NOT_DEF, t.range, codeBlock.scope->getText(t.range).c_str()));
                }
            }
        }
    else //undef
        {
        for(auto& t: define.defs) 
            {
            switch(hash(codeBlock.scope->getText(t.range).c_str()))
                {
                case hash("date_units"):
                    codeBlock.scope->units.removeLongDateUnits(); 
                    break;
                case hash("short_date_units"):
                    codeBlock.scope->units.removeShortDateUnits(); 
                    break;
                case hash("trig"):
                case hash("arithm"):
                case hash("date"):
                case hash("all"):
                    codeBlock.scope->functions.removeFunctions(toFunctionType(hash(codeBlock.scope->getText(t.range).c_str())));
                    break;
                case hash("electric"):
                    codeBlock.scope->units.removeElectric();
                    break;
                case hash("strict"):
                    codeBlock.scope->strict = false;
                    break;
                default:
                    result.errors.push_back(Error(ErrorId::UNDEF_NOT_OK, t.range, codeBlock.scope->getText(t.range).c_str()));
                }
            }

        }
    return result;
    }

Value Resolver::resolveStatement(Statement& stmt)
    {
    Value result;
    if (stmt.codeBlock != nullptr)
        {
        Resolver resolver(*stmt.codeBlock);
        resolver.resolve(output);
        if(!stmt.comment_line.isEmpty())
            result.comment_line = stmt.comment_line;
        if (stmt.error.id != ErrorId::NONE)
            result.errors.push_back(stmt.error);
        return result;//block has no output (for now)
        }
    else 
        {
        if (stmt.node != nullptr)
            result = resolveNode(*stmt.node);
        else
            result.onlyComment = true;
        result.text = stmt.text;
        if(!stmt.comment_line.isEmpty())
            result.comment_line = stmt.comment_line;
        result.mute = stmt.mute;
        if (stmt.error.id != ErrorId::NONE)
            result.errors.push_back(stmt.error);
        result.stmtRange = stmt.range();
        return result;
        }
    }

Value Resolver::resolveNode(const Node& node)
    {
    switch (node.type)
        {
        case NodeType::BINARYOPEXPR: return resolveBinaryOp((const BinaryOpExpr&)node);
        case NodeType::UNARYOPEXPR: return resolveUnaryOp((const UnaryOpExpr&)node);
        case NodeType::STATEMENT: return resolveStatement((Statement&)node);
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
    Value v;
    v.type = ValueType::FUNCTION;
    v.id =  expr.idRange;
    auto fd = codeBlock.scope->getFunction(expr.id);
    if (fd == nullptr)
        return v;

    v.errors = fd->getErrors();
    return v;
    }

Value Resolver::resolveNone(const NoneExpr& expr)
    {
    Value v;
    if(expr.error.id != ErrorId::NONE)
        v.errors.push_back(expr.error);
    if(expr.token.type == TokenType::EOT)
        v.errors.push_back(Error(ErrorId::EOS, expr.range()));
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
        case TokenType::PERCENT: opType = OperatorType::REMAIN; break;
        case TokenType::MODULO: opType = OperatorType::MODULO; break;
        default: break;
        }
    Operator op = codeBlock.scope->globals.operatorDefs.get(OperatorId(a1.type, opType, a2.type, a1.type));
    if (op == nullptr)
        {
        result.errors.push_back(Error(ErrorId::NO_OP, expr.op.range, codeBlock.scope->getText(expr.op.range), to_string(a1.type), to_string(a2.type)));
        return result;
        }
    std::vector<Value> args;
    args.push_back(a1);
    args.push_back(a2);
    result = op(codeBlock.scope->globals, args, expr.op.range);
    if (expr.error.id != ErrorId::NONE)
        result.errors.push_back(expr.error);
    if(result.type ==ValueType::NUMBER && !expr.unit.isClear())
        result.setNumber(result.getNumber().convertToUnit(expr.unit, codeBlock.scope->units));
    if(expr.implicitMult)
        {
        if(expr.n2->is(NodeType::IDEXPR))
            {
            if( codeBlock.scope->units.exists(codeBlock.scope->getText(expr.n2->range())))
                result.errors.push_back(Error(ErrorId::W_UNIT_IS_VAR, expr.n2->range(), codeBlock.scope->getText(expr.n2->range())));
            }
        }

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
            a1.getNumber().range += expr.op.range;
            }
        }
    return a1;
    }

Value Resolver::resolveAssign(const AssignExpr& assign)
    {
    auto result = resolveNode(*assign.expr);
    std::string strId = codeBlock.scope->getText(assign.Id.range);
    if (!codeBlock.scope->varExists(strId))
        {
        if (codeBlock.scope->units.exists(strId))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_UNIT, assign.Id.range, strId));
            }
        else if (codeBlock.scope->functionExists(strId))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_FUNCTION, assign.Id.range, strId));
            }
        codeBlock.scope->emplaceVariable(strId, Value()); //create var, regardless of errors.
        }
    auto& var = getVar(strId);
    if(var.constant)
        result.errors.push_back(Error(ErrorId::CONST_REDEF, assign.Id.range, strId));
    var = result; //do not store the value with the id. The value of a variable is just the value.
    var.errors.clear(); //don't keep the errors with the variables, but only keep thew with the statement's result.
    result.id = assign.Id.range;
    if (assign.error.id != ErrorId::NONE)
        result.errors.push_back(assign.error);
    return result;
    }

Value Resolver::resolveList(const ListExpr& listExpr)
    {
    bool hasDuration = false;
    bool hasOther = false;
    std::vector<Number> numberList;
    for (auto& val : listExpr.list)
        {
        auto value = resolveNode(*val);
        if(value.type != ValueType::NUMBER) 
            return Error(ErrorId::EXPECTED, val->range(), "a numeric expression.");
        numberList.push_back(value.getNumber());
        }
    for (auto& number : numberList)
        {
        if(!codeBlock.scope->units.exists(number.unit.id))
            return Error(ErrorId::UNIT_NOT_DEF, number.unit.getRange(number.range), number.unit.id);
        hasDuration |= codeBlock.scope->units.isUnit(number.unit.id, UnitProperty::DURATION);
        hasOther |= !codeBlock.scope->units.isUnit(number.unit.id, UnitProperty::DURATION);
        }
    if(hasOther && hasDuration)
        return Error(ErrorId::INV_LIST, listExpr.range(), "Units do not match.");
    if(hasDuration)
        return resolveDurationList(numberList);
    else
        return List(numberList);
    }

Value Resolver::resolveDurationList(const std::vector<Number>& numberList)
    {
    bool hasDays = false;
    bool hasMonths = false;
    bool hasYears = false;
    Duration duration;
    for (auto& number : numberList)
        {
        if (number.unit.id == "days")
            {
            if(hasDays)
                return Error(ErrorId::INV_LIST, number.range, "Unit 'days' used more than once.");
            hasDays = true;
            duration.days = (long)number.to_double();
            }
        if (number.unit.id == "months")
            {
            if(hasMonths)
                return Error(ErrorId::INV_LIST, number.range, "Unit 'months' used more than once.");
            hasMonths = true;
            duration.months = (long)number.to_double();
            }
        if (number.unit.id == "years")
            {
            if(hasYears)
                return Error(ErrorId::INV_LIST, number.range, "Unit 'years' used more than once.");
            hasYears = true;
            duration.years = (long)number.to_double();
            }
        }
    return duration;
    }

//wrapper to parse postfixExpressions
Value Resolver::resolvePostfix(const PostfixExpr& pfix)
    {
    if (pfix.error.id != ErrorId::NONE)
        return Value(pfix.error);
    auto val = resolveNode(*pfix.expr);
    if(pfix.postfixId.isNull() && val.type == ValueType::NUMBER)
        val.getNumber().unit = Unit::NONE();
    else 
        switch(hash(codeBlock.scope->getText(pfix.postfixId.range).c_str()))
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
            case hash("to_days"):
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
                    {
                    auto postFixId = codeBlock.scope->getText(pfix.postfixId.range);
                    Unit unit;
                    if (codeBlock.scope->varExists(postFixId)) //TODO: what if both a unit and a var with the same name exists? Currently the var gets precendence.
                        unit = Unit(codeBlock.scope->getVariable(postFixId).getNumber().unit.id, pfix.postfixId.range);
                    else
                        unit = Unit(codeBlock.scope->getText(pfix.postfixId.range), pfix.postfixId.range);

                    val.getNumber() = val.getNumber().convertToUnit(unit, codeBlock.scope->units);
                    }
                else
                    return Value(Error(ErrorId::UNKNOWN_EXPR, pfix.postfixId.range, "Postfix expression not valid here."));
                break;
            }
    //in case of (x.km)m, both postfixId (km) and unit (m) are filled.
    return applyUnit(pfix, val);
    }

Value Resolver::resolveIdExpr(const IdExpr& idExpr)
    {
    Value val;
    std::string strId = codeBlock.scope->getText(idExpr.Id.range);
    if (codeBlock.scope->varExists(strId))
        {
        val = getVar(strId);
        }
    else
        return Value(Error(ErrorId::VAR_NOT_DEF, idExpr.Id.range, strId));

    return applyUnit(idExpr, val);
    }

Value& Resolver::applyUnit(const Node& node, Value& val)
    {
    if(val.type != ValueType::NUMBER)
        return val;
    if (!node.unit.isClear() && !val.getNumber().unit.isClear())
        {
        val.getNumber ()= val.getNumber().convertToUnit(node.unit, codeBlock.scope->units);
        }
    else if (!node.unit.isClear())
        {
        if (codeBlock.scope->units.exists(node.unit.id))
            val.getNumber().unit = node.unit;
        else
            val.errors.push_back(Error(ErrorId::UNIT_NOT_DEF, node.unit.getRange(node.range()), node.unit.id.c_str()));
        }
    return val;
    }

Value Resolver::resolveCall(const CallExpr& callExpr)
    {
    auto fd = codeBlock.scope->getFunction(callExpr.functionName.c_str());
    if (fd == nullptr)
        return Value(Error(ErrorId::FUNC_NOT_DEF, callExpr.functionNameRange, callExpr.functionName));
    if (callExpr.error.id != ErrorId::NONE)
        return Value(callExpr.error);

    // check function arguments:
    if (!fd->isCorrectArgCount(callExpr.arguments->list.size()))
        return Value(Error(ErrorId::FUNC_ARG_MIS, callExpr.functionNameRange, callExpr.functionName));
    std::vector<Error> errors;
    std::vector<Value> args;
    for (auto arg : callExpr.arguments->list)
        {
        auto argVal = resolveNode(*arg);
        errors.insert(errors.begin(), argVal.errors.begin(), argVal.errors.end());
        args.push_back(argVal);
        }
    if (hasRealErrors(errors))
        return Value(errors);

    auto val = fd->call(*codeBlock.scope, args, callExpr.functionNameRange);
    return applyUnit(callExpr, val);
    }

Value Resolver::resolveConst(const ConstExpr& constExpr)
    {
    switch(constExpr.type)
        {
        case ConstType::NUMBER:
            {
            auto v = Value(constExpr.value.numberValue);
            v.getNumber().unit = constExpr.unit;
            if (constExpr.unit.isClear())
                v.getNumber().unit = Unit();
            else
                {
                if (codeBlock.scope->units.exists(v.getNumber().unit.id) == false)
                    {
                    v.getNumber().errors.push_back(Error(ErrorId::UNIT_NOT_DEF, v.getNumber().unit.getRange(v.getNumber().range), v.getNumber().unit.id.c_str()));
                    }
                }
            if (constExpr.error.id != ErrorId::NONE)
                v.errors.push_back(constExpr.error);
            return v;
            }
        case ConstType::FORMATTED_STRING:
            {
            FormattedNumberParser numberParser;
            auto number = numberParser.parse(*codeBlock.scope, codeBlock.scope->getText(constExpr.value.range), constExpr.range());
            if (number.errors.empty())
                {
                return number;
                }

            FormattedDateParser dateParser;
            dateParser.dateFormat = this->dateFormat;
            auto date = dateParser.parse(codeBlock.scope->getText(constExpr.value.range), constExpr.range());
            if(!date.errors.empty())
                date.errors.insert(date.errors.begin(), number.errors.begin(), number.errors.end());
            return Value(date);
            }
        }
    return Value();// just to get rid of the warning
    }

Value Resolver::resolveDateFragment(const Value& val, const Token& fragmentId)
    {
    Value newValue;
    if(val.type != ValueType::TIMEPOINT)
        return Value(Error(ErrorId::DATE_FRAG_NO_DATE, fragmentId.range, codeBlock.scope->getText(fragmentId.range)));
    const Date date = val.getDate();
    switch (hash(codeBlock.scope->getText(fragmentId.range).c_str()))
        {
        case hash("day"):
            newValue = Value(Number(date.day, 0, fragmentId.range)); break;
        case hash("month"):
            newValue = Value(Number((double)date.month, 0, fragmentId.range)); break;
        case hash("year"):
            newValue = Value(Number(date.year, 0, fragmentId.range)); break;
        default:
            return Value(Error(ErrorId::DATE_INV_FRAG, fragmentId.range, codeBlock.scope->getText(fragmentId.range)));
        }
    return newValue;
    }

Value Resolver::resolveDurationFragment(const Value& val, const Token& fragmentId)
    {
    Value newValue;
    if(val.type != ValueType::DURATION)
        return Value(Error(ErrorId::DATE_FRAG_NO_DURATION, fragmentId.range, codeBlock.scope->getText(fragmentId.range)));
    const auto dur = val.getDuration();
    switch (hash(codeBlock.scope->getText(fragmentId.range).c_str()))
        {
        case hash("to_days"):
            newValue = Value(Number(dur.to_days(), Unit("days", fragmentId.range), NumFormat::DEC, fragmentId.range)); break;
        case hash("days"):
            newValue = Value(Number(dur.days, Unit("days", fragmentId.range), NumFormat::DEC, fragmentId.range)); break;
        case hash("months"):
            newValue = Value(Number((double)dur.months, Unit("months", fragmentId.range), NumFormat::DEC, fragmentId.range)); break;
        case hash("years"):
            newValue = Value(Number(dur.years, Unit("years", fragmentId.range), NumFormat::DEC, fragmentId.range)); break;
        default:
            return Value(Error(ErrorId::DUR_INV_FRAG, fragmentId.range, codeBlock.scope->getText(fragmentId.range)));
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
    return codeBlock.scope->getVariable(id);
    }
