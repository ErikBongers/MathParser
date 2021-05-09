using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MathParserLib
    {
    public enum ErrorId
        {
        NONE = 0,
        UNKNOWN_EXPR,
        VAR_NOT_DEF,
        FUNC_NOT_DEF,
        FUNC_ARG_MIS,
        W_DIV_IMPL_MULT,
        W_POW_IMPL_MULT,
        }

    public class Error
        {
        public ErrorId id;
        public string errorMsg;
        public int line;
        public int pos;

        public Error(ErrorId id, params object[] args)
            {
            this.id = id;
            this.errorMsg = String.Format(ErrorDefs.getMsg(id), args);
            }
        }

    public static class ErrorDefs
        {
        public static Dictionary<ErrorId, string> errorDefs = new Dictionary<ErrorId, string>
            {
                {ErrorId.UNKNOWN_EXPR,  "Unknown expression."},
                {ErrorId.VAR_NOT_DEF,  "variable \"{0}\" is not defined."},
                {ErrorId.FUNC_NOT_DEF,  "function \"{0}\" is not defined."},
                {ErrorId.FUNC_ARG_MIS,  "argument(s) for function \"{0}\" missing."},
                {ErrorId.W_DIV_IMPL_MULT,  "Warning: ambiguous expression: division followed by implicit multiplication."},
                {ErrorId.W_POW_IMPL_MULT,  "Warning: ambiguous expression: exponential combined with implicit multiplication."},
            };

        public static string getMsg(ErrorId id)
            {
            return errorDefs[id];
            }
        }

    public class Value
        {
        public Token id = null; //optional, in case of a variable.
        public double number = 0;
        public Token unit;
        public List<Error> errors = new List<Error>();

        public Value(ErrorId errorId, params object[] args)
            {
            errors.Add(new Error(errorId, args));
            }

        public Value(double d)
            {
            this.number = d;
            }
        public Value(double d, Token u)
            {
            this.number = d;
            this.unit = u;
            }
        public Value(Token id, double d, Token u)
            {
            this.id = id;
            this.number = d;
            this.unit = u;
            }
        public override string ToString()
            {
            return number.ToString("0.#######") + unit;
            }
        public string ToString(string format)
            {
            string str = "";
            if (unit != null && unit.Type == TokenType.RAD)
                str = new Value(number / Math.PI * 180, new Token(TokenType.DEG, "deg")).ToString();
            else
                str = number.ToString(format) + unit;
            if (errors.Count > 0)
                {
                errors.ForEach(err => str += "  <<< " + err.errorMsg);
                }
            return str;
            }
        public static implicit operator Value(double d) => new Value(d);
        public static Value operator +(Value a, Value b)
            {
            double d = a.number + b.number;
            Token unit = a.unit;
            if (unit == null)
                unit = b.unit;
            var val = new Value(d, unit);
            val.errors.AddRange(a.errors);
            val.errors.AddRange(b.errors);
            return val;
            }
        public static Value operator -(Value a, Value b)
            {
            double d = a.number - b.number;
            Token unit = a.unit;
            if (unit == null)
                unit = b.unit;
            var val = new Value(d, unit);
            val.errors.AddRange(a.errors);
            val.errors.AddRange(b.errors);
            return val;
            }
        public static Value operator *(Value a, Value b)
            {
            double d = a.number * b.number;
            Token unit = a.unit;
            if (unit == null)
                unit = b.unit;
            var val = new Value(d, unit);
            val.errors.AddRange(a.errors);
            val.errors.AddRange(b.errors);
            return val;
            }
        public static Value operator /(Value a, Value b)
            {
            double d = a.number / b.number;
            Token unit = a.unit;
            if (unit == null)
                unit = b.unit;
            var val = new Value(d, unit);
            val.errors.AddRange(a.errors);
            val.errors.AddRange(b.errors);
            return val;
            }
        public static Value operator ^(Value a, Value b) //REDEFINITION of XOR to POWER!!!
            {
            double d = Math.Pow(a.number, b.number);
            Token unit = a.unit;
            var val = new Value(d, unit);
            val.errors.AddRange(a.errors);
            val.errors.AddRange(b.errors);
            return val;
            }
        }

    public class Resolver
        {
        Parser parser;
        public Resolver(Parser p)
            {
            this.parser = p;
            }

        public Value resolve(Statement stmt)
            {
            return resolveStatement(stmt);
            }

        private Value resolveNode(Node node)
            {
            switch (node)
                {
                case AddExpr addExpr: return resolveAdd(addExpr);
                case MultExpr multExpr: return resolveMult(multExpr);
                case Statement stmt: return resolveStatement(stmt);
                case AssignExpr assignExpr: return resolveAssign(assignExpr);
                case PowerExpr powerExpr: return resolvePower(powerExpr);
                case PrimaryExpr primExpr: return resolvePrim(primExpr);
                case ConstExpr constExpr: return resolveConst(constExpr);
                default: return new Value(ErrorId.UNKNOWN_EXPR);
                }
            }
        private Value resolveStatement(Statement stmt)
            {
            if (stmt.assignExpr != null)
                return resolveNode(stmt.assignExpr);
            else
                return resolveNode(stmt.addExpr);
            }

        public Value resolveAssign(AssignExpr assign)
            {
            var result = resolveNode(assign.addExpr);
            result.id = assign.Id;
            if (assign.error != null)
                result.errors.Add(assign.error);
            return result;
            }

        public Value resolveAdd(AddExpr addExpr)
            {
            Value a1 = resolveNode(addExpr.a1);
            Value a2 = resolveNode(addExpr.a2);
            Value result;
            if (addExpr.op.Type == TokenType.PLUS)
                result = a1 + a2;
            else
                result = a1 - a2;
            if (addExpr.error != null)
                result.errors.Add(addExpr.error);
            return result;
            }

        private Value resolveMult(MultExpr multExpr)
            {
            Value m1 = resolveNode(multExpr.m1);
            Value m2 = resolveNode(multExpr.m2);
            Value result;
            if (multExpr.op.Type == TokenType.MULT)
                result = m1 * m2;
            else
                result = m1 / m2;
            if (multExpr.error != null)
                result.errors.Add(multExpr.error);
            return result;
            }

        private Value resolvePower(PowerExpr powerExpr)
            {
            Value prim = resolveNode(powerExpr.p2);
            if (powerExpr.p1 == null)
                return prim;
            Value power = resolveNode(powerExpr.p1);
            var result = power ^ prim;
            if (powerExpr.error != null)
                result.errors.Add(powerExpr.error);
            return result;
            }

        private Value resolvePrim(PrimaryExpr prim)
            {
            if (prim.addExpr != null)
                return resolveNode(prim.addExpr);
            else if (prim.Id != null)
                {
                if (parser.ids.ContainsKey(prim.Id.StringValue))
                    return resolveNode(parser.ids[prim.Id.StringValue].addExpr);
                else
                    return new Value(ErrorId.VAR_NOT_DEF, prim.Id);
                }
            else if (prim.callExpr != null)
                {
                var callExpr = (CallExpr)prim.callExpr;
                var f = Function.get(callExpr.functionName.StringValue);
                if (f == null)
                    return new Value(ErrorId.FUNC_NOT_DEF, callExpr.functionName);
                if (((CallExpr)prim.callExpr).argument == null)
                    return new Value(ErrorId.FUNC_ARG_MIS, callExpr.functionName);
                f.clearArgs();
                f.addArg(resolveNode(((CallExpr)prim.callExpr).argument));
                return f.execute();
                }
            else
                return new Value(ErrorId.UNKNOWN_EXPR);
            }

        private Value resolveConst(ConstExpr constExpr)
            {
            return new Value(constExpr.constNumber.NumberValue, constExpr.unit);
            }

        private string FormatError(string errorMsg, params object[] args)
            {
            return String.Format(errorMsg, args);
            }
        }

    }
