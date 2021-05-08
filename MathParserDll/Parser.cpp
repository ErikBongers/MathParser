#include "pch.h"
#include "Parser.h"
#include "Function.h"

void Parser::parse()
    {
    ids.emplace("pi", Variable{ Token(TokenType::ID, "pi"), new ConstExpr(Token(TokenType::NUMBER, M_PI)) });
    while (peekToken().type != TokenType::EOT)
        {
        statements.push_back(parseStatement());
        nextToken();
        if (currentToken.type != TokenType::SEMI_COLON)
            pushBackLastToken();//continue, although what follows is an error
        }
    }

Statement* Parser::parseStatement()
    {
    Statement* stmt = new Statement();
    stmt->assignExpr = parseAssignExpr();
    if (stmt->assignExpr->isNull())
        {
        stmt->addExpr = parseAddExpr();
        }
    return stmt;
    }

Node* Parser::parseAssignExpr()
    {
    nextToken();
    if (currentToken.type == TokenType::ID)
        {
        auto id = currentToken;
        auto t = peekToken();
        if (t.type == TokenType::EQ)
            {
            nextToken();//consume the EQ
            AssignExpr* assign = new AssignExpr();
            assign->Id = id;
            assign->addExpr = parseAddExpr();
            ids.emplace(assign->Id.stringValue, Variable{ assign->Id, assign->addExpr });
            return assign;
            }
        else
            {
            pushBackLastToken(); //pushing back the ID, not the EQ!
            return nullptr;
            }
        }
    else
        {
        pushBackLastToken();
        return nullptr;
        }
    }

Node* Parser::parseAddExpr()
    {
    AddExpr* addExpr = new AddExpr();
    addExpr->a1 = parseMultExpr();
    auto t = nextToken();
    while (t.type == TokenType::PLUS || t.type == TokenType::MIN)
        {
        addExpr->op = t;
        addExpr->a2 = parseMultExpr();
        // prepare next iteration.
        auto oldAddExpr = addExpr;
        addExpr = new AddExpr();
        addExpr->a1 = oldAddExpr;
        t = nextToken();
        }
    pushBackLastToken();
    return addExpr->a1; // last iteration incomplete.
    }

Node* Parser::parseMultExpr()
    {
    MultExpr* multExpr = new MultExpr();
    multExpr->m1 = parsePowerExpr();
    auto t = nextToken();
    while (t.type == TokenType::MULT || t.type == TokenType::DIV)
        {
        multExpr->op = t;
        multExpr->m2 = parsePowerExpr();
        // prepare next iteration.
        auto oldMultExpr = multExpr;
        multExpr = new MultExpr();
        multExpr->m1 = oldMultExpr;
        //give warning if expr of form a/2b:
        if (multExpr->m1->is(NodeType::MULTEXPR))
            {
            MultExpr* div = static_cast<MultExpr*>(multExpr->m1);
            if (div->op.type == TokenType::DIV)
                {
                if (div->m2->is(NodeType::MULTEXPR))
                    {
                    MultExpr* m2 = static_cast<MultExpr*>(div->m2);
                    if (m2->implicitMult)
                        {
                        div->error = Error(ErrorId::W_DIV_IMPL_MULT);
                        }
                    }
                }
            }
        t = nextToken();
        }
    pushBackLastToken();
    return multExpr->m1; // last iteration incomplete.
    }

Node* Parser::parsePowerExpr()
    {
    PowerExpr* powerExpr = new PowerExpr();
    powerExpr->p2 = parseImplicitMult();
    auto t = nextToken();
    if (t.type != TokenType::POWER)
        {
        pushBackLastToken();
        return powerExpr->p2;
        }
    while (t.type == TokenType::POWER)
        {
        PowerExpr* newPowerExpr = new PowerExpr();
        newPowerExpr->p1 = powerExpr;
        newPowerExpr->p2 = parseImplicitMult();
        powerExpr = newPowerExpr;
        if (powerExpr->p1->is(NodeType::MULTEXPR))
            {
            MultExpr* m1 = static_cast<MultExpr*>(powerExpr->p1);
            if (m1->implicitMult || (powerExpr->p2->is(NodeType::MULTEXPR)))
                {
                MultExpr* m2 = static_cast<MultExpr*>(powerExpr->p2);
                if (m2->implicitMult)
                    {
                    powerExpr->error = Error(ErrorId::W_POW_IMPL_MULT);
                    }
                }
            }
        t = nextToken();
        }
    pushBackLastToken();
    return powerExpr;
    }

Node* Parser::parseImplicitMult()
    {
    Node* n1 = parsePrimaryExpr();
    auto t = nextToken();
    while ((t.type == TokenType::ID
            || t.type == TokenType::NUMBER)
           || t.type == TokenType::PAR_OPEN)
        {
        pushBackLastToken();
        auto m = new MultExpr();
        m->m1 = n1;
        m->op = Token(TokenType::MULT, '*');
        m->m2 = parsePrimaryExpr();
        m->implicitMult = true;
        n1 = m;
        t = nextToken();
        }
    pushBackLastToken();
    return n1;
    }

Node* Parser::parsePrimaryExpr()
    {
    PrimaryExpr* primExpr = new PrimaryExpr();//TODO: get rid of this.
    auto t = nextToken();
    if (t.type == TokenType::NUMBER)
        {
        pushBackLastToken();
        return parseConst(false);
        }
    else if (t.type == TokenType::MIN)
        {
        if (peekToken().type == TokenType::NUMBER)
            {
            return parseConst(true);
            }
        //else error?
        }
    else if (t.type == TokenType::ID)
        {
        if (Function::exists(t.stringValue))
            {
            primExpr->callExpr = parseCallExpr(t);
            }
        else
            primExpr->Id = t;
        }
    else if (t.type == TokenType::PAR_OPEN)
        {
        primExpr->addExpr = parseAddExpr();
        t = nextToken();
        //if (t.type != TokenType::PAR_CLOSE)
        //    primExpr.errorPos = 1;//todo
        return primExpr; //always return a 'wrapped' expression as a primExpr, not it's content, for analysis and warnings.
        }
    return primExpr;
    }

ConstExpr* Parser::parseConst(bool negative)
    {
    ConstExpr* constExpr = new ConstExpr();
    auto t = nextToken();
    t.numberValue = (negative ? -1 : 1) * t.numberValue;
    constExpr->constNumber = t;
    t = nextToken();
    if (t.type == TokenType::RAD || t.type == TokenType::DEG)
        constExpr->unit = t;
    else
        pushBackLastToken();
    return constExpr;
    }

CallExpr* Parser::parseCallExpr(Token functionName)
    {
    CallExpr* callExpr = new CallExpr();
    callExpr->functionName = functionName;
    auto t = nextToken();
    if (t.type == TokenType::PAR_OPEN)
        callExpr->argument = parseAddExpr();
    t = nextToken();
    if (t.type != TokenType::PAR_CLOSE)
        pushBackLastToken();
    return callExpr;
    }

Token Parser::peekToken()
    {
    if (lastToken.type != TokenType::NULLPTR)
        return lastToken;
    else
        return tok.peek();
    }

Token Parser::nextToken()
    {
    if (lastToken.type != TokenType::NULLPTR)
        {
        currentToken = lastToken;
        lastToken = Token(TokenType::NULLPTR);
        }
    else
        {
        currentToken = tok.next();
        }
    return currentToken;
    }

void Parser::pushBackLastToken()
    {
    if (lastToken.type != TokenType::NULLPTR)
        throw new std::exception("Cannot push back a second token.");

    lastToken = currentToken;
    }
