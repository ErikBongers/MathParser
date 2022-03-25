#include "pch.h"
#include "Resolver.h"
#include "Function.h"
#include "Number.h"
#include "OperatorDef.h"
#include "trim.h"
#include <sstream>

std::string Resolver::result = "";

Resolver::Resolver(Parser& parser, UnitDefs& unitDefs, OperatorDefs& operatorDefs) : parser(parser), unitDefs(unitDefs), operatorDefs(operatorDefs) 
    {
    }

void Resolver::resolve()
    {
    variables.clear();
    auto PI = Value(Number(M_PI, 0, 0, 0));
    PI.constant = true;
    variables.emplace("PI", PI);
    variables.emplace("pi", PI);
    std::vector<std::string> jsonRes;
    parser.parse();
    for (auto& stmt : parser.statements)
        {
        auto result = resolveStatement(*stmt);
        if (stmt->node != nullptr)
            {
            if(stmt->node->type != NodeType::DEFINE || result.errors.size() != 0)
                jsonRes.push_back(result.to_json()); //don't output a define statement unless it has errors
            }
        else
            jsonRes.push_back(result.to_json());
        }
    result = "";
    for (auto& s : jsonRes)
        result += s + ",";
    if(result.size() > 0)
        result.resize(result.size()-1); //remove last comma;
    result = "{\"result\" : [" + result + "]}";
    }

Value Resolver::resolveDefine(const Define& define)
    {
    Value result;
    std::string options = rtrim_copy(define.def.stringValue);
    std::istringstream iss(options);
    std::string item;
    while (std::getline(iss, item, ' ')) {
        if(item == "date_units")
            this->unitDefs.addDateUnits();
        else if(item == "short_date_units")
            this->unitDefs.addShortDateUnits();
        else
            result.errors.push_back(Error(ErrorId::DEFINE_NOT_DEF, define.def.line, define.def.pos, options));
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
    return result;
    }

Value Resolver::resolveNode(const Node& node)
    {
    switch (node.type)
        {
        case NodeType::BINARYOPEXPR: return resolveBinaryOp((const BinaryOpExpr&)node);
        case NodeType::STATEMENT: return resolveStatement((const Statement&)node);
        case NodeType::ASSIGNMENT: return resolveAssign((const AssignExpr&)node);
        case NodeType::IDEXPR: return resolvePrim((const IdExpr&)node);
        case NodeType::POSTFIXEXPR: return resolvePostfix((const PostfixExpr&)node);
        case NodeType::CONSTEXPR: return resolveConst((const ConstExpr&)node);
        case NodeType::CALLEXPR: return resolveCall((const CallExpr&)node);
        case NodeType::DEFINE: return resolveDefine((const Define&)node);
        default: return Value(Error(ErrorId::UNKNOWN_EXPR, 0, 0));
        }
    }

Value Resolver::resolveBinaryOp(const BinaryOpExpr& addExpr)
    {
    Value a1 = resolveNode(*addExpr.n1);
    Value a2 = resolveNode(*addExpr.n2);
    Value result;

    OperatorType opType = OperatorType::NONE;
    switch (addExpr.op.type)
        {
        case TokenType::PLUS: opType = OperatorType::PLUS; break;
        case TokenType::MIN: opType = OperatorType::MIN; break;
        case TokenType::MULT: opType = OperatorType::MULT; break;
        case TokenType::DIV: opType = OperatorType::DIV; break;
        case TokenType::POWER: opType = OperatorType::POW; break;
        default: break;
        }
    OperatorDef* op = operatorDefs.get(OperatorId(a1.type, opType, a2.type, a1.type));
    if (op == nullptr)
        {
        result.errors.push_back(Error(ErrorId::NO_OP, addExpr.op.line, addExpr.op.pos, addExpr.op.stringValue, to_string(a1.type), to_string(a2.type)));
        return result;
        }
    std::vector<Value> args;
    args.push_back(a1);
    args.push_back(a2);
    result = op->call(args, a1.getNumber().line, a1.getNumber().pos);
    if (addExpr.error.id != ErrorId::NONE)
        result.errors.push_back(addExpr.error);
    if(!addExpr.unit.isClear())
        result.setNumber(result.getNumber().convertToUnit(addExpr.unit, unitDefs));
    return result;
    }

Value Resolver::resolveAssign(const AssignExpr& assign)
    {
    auto result = resolveNode(*assign.expr);
    if (variables.count(assign.Id.stringValue) == 0)
        {
        if (unitDefs.exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_UNIT, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
            }
        else if (parser.functionDefs.exists(assign.Id.stringValue))
            {
            result.errors.push_back(Error(ErrorId::W_VAR_IS_FUNCTION, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
            }
        variables.emplace(assign.Id.stringValue, Value()); //create var, regardless of errors.
        }
    auto& var = getVar(assign.Id.stringValue);
    if(var.constant)
        result.errors.push_back(Error(ErrorId::CONST_REDEF, assign.Id.line, assign.Id.pos, assign.Id.stringValue));
    var = result; //do not store the value with the id. The value of a variable is just the value.
    var.errors.clear(); //don't keep the errors with the variables, but only keep thew with the statement's result.
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
        val.getNumber().unit = Unit::CLEAR();
    else if(pfix.postfixId.stringValue == "bin")
        val.getNumber().numFormat = NumFormat::BIN;
    else if(pfix.postfixId.stringValue == "hex")
        val.getNumber().numFormat = NumFormat::HEX;
    else if(pfix.postfixId.stringValue == "dec")
        val.getNumber().numFormat = NumFormat::DEC;
    else
        val.getNumber ()= val.getNumber().convertToUnit(pfix.postfixId.stringValue, unitDefs);
    //in case of (x.km)m, both postfixId (km) and unit (m) are filled.
    return applyUnit(pfix, val);
    }

Value Resolver::resolvePrim(const IdExpr& prim)
    {
    Value val;
    if (prim.Id.type != TokenType::NULLPTR )
        {
        auto found = variables.find(prim.Id.stringValue);
        if (found != variables.end())
            {
            val = getVar(prim.Id.stringValue);
            }
        else
            return Value(Error(ErrorId::VAR_NOT_DEF, prim.Id.line, prim.Id.pos, prim.Id.stringValue.c_str()));
        }
    else
        return Value(Error(ErrorId::UNKNOWN_EXPR, 0, 0)); //TODO: this should never happen? -> allow only creation of prim with one of the above sub-types.

    return applyUnit(prim, val);
    }

Value& Resolver::applyUnit(const Node& node, Value& val)
    {
    if (!node.unit.isClear() && !val.getNumber().unit.isClear())
        {
        val.getNumber ()= val.getNumber().convertToUnit(node.unit, unitDefs);
        }
    else if (!node.unit.isClear())
        val.getNumber().unit = node.unit;
    return val;
    }

Value Resolver::resolveCall(const CallExpr& callExpr)
    {
    auto fd = parser.functionDefs.get(callExpr.functionName.stringValue.c_str());
    if (fd == nullptr)
        return Value(Error(ErrorId::FUNC_NOT_DEF, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str()));
    if (callExpr.error.id != ErrorId::NONE)
        return Value(callExpr.error);

    // check function arguments:
    if (!fd->isCorrectArgCount(callExpr.arguments.size()))
        return Value(Error(ErrorId::FUNC_ARG_MIS, callExpr.functionName.line, callExpr.functionName.pos, callExpr.functionName.stringValue.c_str()));
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
    if(constExpr.type == ValueType::NUMBER)
        {
        auto v = Value(constExpr.value.numberValue);
        v.getNumber().unit = constExpr.unit;
        if (constExpr.unit.isClear())
            v.getNumber().unit = Unit();
        else
            {
            if (unitDefs.exists(v.getNumber().unit.id) == false)
                {
                v.getNumber().errors.push_back(Error(ErrorId::UNIT_NOT_DEF, v.getNumber().unit.line, v.getNumber().unit.pos, v.getNumber().unit.id.c_str()));
                }
            }
        if (constExpr.error.id != ErrorId::NONE)
            v.errors.push_back(constExpr.error);
        return v;
        }
    else
        {
        return Value(DateParser(constExpr.value.stringValue, constExpr.value.line, constExpr.value.pos).parse());
        }
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
