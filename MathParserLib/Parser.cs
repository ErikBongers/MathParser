using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MathParserLib
    {
    public class Node
        {
        public Error error;
        }

    public class ConstExpr : Node
        {
        public Token constNumber;
        public Token unit;
        }

    public class PrimaryExpr : Node
        {
        public Token Id;
        public Node addExpr;
        public Node callExpr;
        }

    public class CallExpr : Node
        {
        public Token functionName;
        public Node argument; //TODO: make argument list
        }

    public class MultExpr : Node
        {
        public Node m1;
        public Token op;
        public Node m2;
        public bool implicitMult = false;
        }

    public class PowerExpr : Node
        {
        public Node p1;
        //public Token op; == '^'
        public Node p2;
        }

    public class AddExpr : Node
        {
        public Node a1;
        public Token op;
        public Node a2;
        }

    public class AssignExpr : Node
        {
        public Token Id;
        public Node addExpr;
        }

    public class Statement : Node
        {
        public Node assignExpr;
        public Node addExpr;
        }

    public class Variable
        {
        public Token name;
        public Node addExpr;
        //public Value value;
        //public Function;
        }

    public class Parser
        {
        Tokenizer tok;
        public List<Statement> statements = new List<Statement>();
        public Dictionary<string, Variable> ids = new Dictionary<string, Variable>();

        public Parser(string stream)
            {
            this.tok = new Tokenizer(stream);
            }

        public List<Statement> parse()
            {
            ids.Add("pi", new Variable { addExpr = new ConstExpr { constNumber = new Token(TokenType.NUMBER, Math.PI) } });
            while (peekToken().Type != TokenType.EOF)
                {
                statements.Add(parseStatement());
                nextToken();
                if (currentToken.Type != TokenType.SEMI_COLON)
                    pushBackLastToken();//continue, although what follows is an error
                }
            return statements;
            }

        private Statement parseStatement()
            {
            Statement stmt = new Statement();
            stmt.assignExpr = parseAssignExpr();
            if (stmt.assignExpr == null)
                {
                stmt.addExpr = parseAddExpr();
                }
            return stmt;
            }

        private AssignExpr parseAssignExpr()
            {
            nextToken();
            if (currentToken.Type == TokenType.ID)
                {
                var id = currentToken;
                var t = peekToken();
                if (t.Type == TokenType.EQ)
                    {
                    nextToken();//consume the EQ
                    AssignExpr assign = new AssignExpr();
                    assign.Id = id;
                    assign.addExpr = parseAddExpr();
                    ids.Add(assign.Id.StringValue, new Variable { name = assign.Id, addExpr = assign.addExpr });
                    return assign;
                    }
                else
                    {
                    pushBackLastToken(); //pushing back the ID, not the EQ!
                    return null;
                    }
                }
            else
                {
                pushBackLastToken();
                return null;
                }
            }

        private Node parseAddExpr()
            {
            AddExpr addExpr = new AddExpr();
            addExpr.a1 = parseMultExpr();
            var t = nextToken();
            while (t.Type == TokenType.PLUS || t.Type == TokenType.MIN)
                {
                addExpr.op = t;
                addExpr.a2 = parseMultExpr();
                addExpr = new AddExpr { a1 = addExpr }; // prepare next iteration.
                t = nextToken();
                }
            pushBackLastToken();
            return addExpr.a1; // last iteration incomplete.
            }

        private Node parseMultExpr()
            {
            MultExpr multExpr = new MultExpr();
            multExpr.m1 = parsePowerExpr();
            var t = nextToken();
            while (t.Type == TokenType.MULT || t.Type == TokenType.DIV)
                {
                multExpr.op = t;
                multExpr.m2 = parsePowerExpr();
                multExpr = new MultExpr { m1 = multExpr }; // prepare next iteration.
                //give warning if expr of form a/2b:
                if (multExpr.m1 is MultExpr div && div.op.Type == TokenType.DIV && (div.m2 is MultExpr m2 && m2.implicitMult))
                    {
                    div.error = new Error(ErrorId.W_DIV_IMPL_MULT);
                    }
                t = nextToken();
                }
            pushBackLastToken();
            return multExpr.m1; // last iteration incomplete.
            }

        private Node parsePowerExpr()
            {
            PowerExpr powerExpr = new PowerExpr();
            powerExpr.p2 = parseImplicitMult();
            var t = nextToken();
            if (t.Type != TokenType.POWER)
                {
                pushBackLastToken();
                return powerExpr.p2;
                }
            while (t.Type == TokenType.POWER)
                {
                PowerExpr newPowerExpr = new PowerExpr { p1 = powerExpr };
                newPowerExpr.p2 = parseImplicitMult();
                powerExpr = newPowerExpr;
                if ((powerExpr.p1 is MultExpr m1 && m1.implicitMult) || (powerExpr.p2 is MultExpr m2 && m2.implicitMult))
                    {
                    powerExpr.error = new Error(ErrorId.W_POW_IMPL_MULT);
                    }
                t = nextToken();
                }
            pushBackLastToken();
            return powerExpr;
            }

        private Node parseImplicitMult()
            {
            Node n1 = parsePrimaryExpr();
            var t = nextToken();
            while ((t.Type == TokenType.ID
                || t.Type == TokenType.NUMBER)
                || t.Type == TokenType.PAR_OPEN)
                {
                pushBackLastToken();
                var m = new MultExpr { m1 = n1, op = new Token(TokenType.MULT, '*'), m2 = parsePrimaryExpr(), implicitMult = true };
                n1 = m;
                t = nextToken();
                }
            pushBackLastToken();
            return n1;
            }

        private Node parsePrimaryExpr()
            {
            PrimaryExpr primExpr = new PrimaryExpr();//TODO: get rid of this.
            var t = nextToken();
            if (t.Type == TokenType.NUMBER)
                {
                pushBackLastToken();
                return parseConst(false);
                }
            else if (t.Type == TokenType.MIN)
                {
                if (peekToken().Type == TokenType.NUMBER)
                    {
                    return parseConst(true);
                    }
                //else error?
                }
            else if (t.Type == TokenType.ID)
                {
                if (Function.exists(t.StringValue))
                    {
                    primExpr.callExpr = parseCallExpr(t);
                    }
                else
                    primExpr.Id = t;
                }
            else if (t.Type == TokenType.PAR_OPEN)
                {
                primExpr.addExpr = parseAddExpr();
                t = nextToken();
                //if (t.Type != TokenType.PAR_CLOSE)
                //    primExpr.errorPos = 1;//todo
                return primExpr; //always return a 'wrapped' expression as a primExpr, not it's content, for analysis and warnings.
                }
            return primExpr;
            }

        private ConstExpr parseConst(bool negative)
            {
            ConstExpr constExpr = new ConstExpr();
            var t = nextToken();
            t.NumberValue = (negative ? -1 : 1) * t.NumberValue;
            constExpr.constNumber = t;
            t = nextToken();
            if (t.Type == TokenType.RAD || t.Type == TokenType.DEG)
                constExpr.unit = t;
            else
                pushBackLastToken();
            return constExpr;
            }

        private CallExpr parseCallExpr(Token functionName)
            {
            CallExpr callExpr = new CallExpr { functionName = functionName };
            var t = nextToken();
            if (t.Type == TokenType.PAR_OPEN)
                callExpr.argument = parseAddExpr();
            t = nextToken();
            if (t.Type != TokenType.PAR_CLOSE)
                pushBackLastToken();
            return callExpr;
            }

        private Token currentToken = null;
        private Token lastToken = null;

        private Token peekToken()
            {
            if (lastToken != null)
                return lastToken;
            else
                return tok.peek();
            }

        private Token nextToken()
            {
            if (lastToken != null)
                {
                currentToken = lastToken;
                lastToken = null;
                }
            else
                {
                currentToken = tok.next();
                }
            return currentToken;
            }

        private void pushBackLastToken()
            {
            if (lastToken != null)
                throw new Exception("Cannot push back a second token.");

            lastToken = currentToken;
            }
        }
    }
