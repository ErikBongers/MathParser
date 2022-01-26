#include "pch.h"
#include "Parser.h"
#include "Function.h"

void Parser::parse()
    {
    ConstExpr* pConst = createConst();
    pConst->constNumber = Token(TokenType::NUMBER, M_PI);
    ids.emplace("pi", Variable{ Token(TokenType::ID, "pi"), pConst});
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
    Statement* stmt = createStatement();
    stmt->assignExpr = parseAssignExpr();
    if (stmt->assignExpr == nullptr)
        stmt->addExpr = parseAddExpr();
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
            AssignExpr* assign = createAssign();
            assign->Id = id;
            assign->expr = parseAddExpr();
            ids.emplace(assign->Id.stringValue, Variable{ assign->Id, assign->expr });
            return assign;
            }
        else if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV)
            {
            nextToken();//consume the EQ
            AssignExpr* assign = createAssign();
            assign->Id = id;
            //prepare components to fabricate the new add or mult expression.
            PrimaryExpr* idExpr = createPrimary();
            idExpr->Id = id;
            TokenType oper;
            switch (t.type)
                {
                case TokenType::EQ_PLUS: oper = TokenType::PLUS; break;
                case TokenType::EQ_MIN: oper = TokenType::MIN; break;
                case TokenType::EQ_MULT: oper = TokenType::MULT; break;
                case TokenType::EQ_DIV: oper = TokenType::DIV; break;
                }

            //build expression
            if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN)
                {
                AddExpr* addExpr = createAdd();
                addExpr->a1 = idExpr;
                addExpr->op = Token(oper);
                addExpr->a2 = parseAddExpr();
                assign->expr = addExpr;
                }
            else
                {
                MultExpr* multExpr = createMult();
                multExpr->m1 = idExpr;
                multExpr->op = Token(oper);
                multExpr->m2 = parseAddExpr(); //note the parseAddExpr instead of parseMultExpr(), as the right=hand of the assign is similar to an expression between parenthesis.
                assign->expr = multExpr;
                }
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
    AddExpr* addExpr = createAdd();
    addExpr->a1 = parseMultExpr();
    auto t = nextToken();
    while (t.type == TokenType::PLUS || t.type == TokenType::MIN)
        {
        addExpr->op = t;
        addExpr->a2 = parseMultExpr();
        // prepare next iteration.
        auto oldAddExpr = addExpr;
        addExpr = createAdd();
        addExpr->a1 = oldAddExpr;
        t = nextToken();
        }
    pushBackLastToken();
    return addExpr->a1; // last iteration incomplete.
    }

Node* Parser::parseMultExpr()
    {
    MultExpr* multExpr = createMult();
    multExpr->m1 = parsePowerExpr();
    auto t = nextToken();
    while (t.type == TokenType::MULT || t.type == TokenType::DIV)
        {
        multExpr->op = t;
        multExpr->m2 = parsePowerExpr();
        // prepare next iteration.
        auto oldMultExpr = multExpr;
        multExpr = createMult();
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
    PowerExpr* powerExpr = createPower();
    powerExpr->p2 = parseImplicitMult();
    auto t = nextToken();
    if (t.type != TokenType::POWER)
        {
        pushBackLastToken();
        return powerExpr->p2;
        }
    while (t.type == TokenType::POWER)
        {
        PowerExpr* newPowerExpr = createPower();
        newPowerExpr->p1 = powerExpr;
        newPowerExpr->p2 = parseImplicitMult();
        powerExpr = newPowerExpr;
        if ((powerExpr->p1->is(NodeType::MULTEXPR) && static_cast<MultExpr*>(powerExpr->p1)->implicitMult)
            || (powerExpr->p2->is(NodeType::MULTEXPR) && static_cast<MultExpr*>(powerExpr->p2)->implicitMult))
            {
            powerExpr->error = Error(ErrorId::W_POW_IMPL_MULT);
            }
        t = nextToken();
        }
    pushBackLastToken();
    return powerExpr;
    }

Node* Parser::parseImplicitMult()
    {
    Node* n1 = parseUnitExpr();
    auto t = nextToken();
    while ((t.type == TokenType::ID
            || t.type == TokenType::NUMBER)
           || t.type == TokenType::PAR_OPEN)
        {
        pushBackLastToken();
        auto m = createMult();
        m->m1 = n1;
        m->op = Token(TokenType::MULT, '*');
        m->m2 = parseUnitExpr();
        m->implicitMult = true;
        n1 = m;
        t = nextToken();
        }
    pushBackLastToken();
    return n1;
    }

Node* Parser::parseUnitExpr()
    {
    Node* node = parsePostFixExpr();
    auto t = nextToken();
    if (t.type == TokenType::ID)
        {
        if (ids.count(t.stringValue) != 0)
            pushBackLastToken(); //a known id: assuming an implicit mult, here.
        else
            node->unit = t; //no known id: assuming a unit.
        }
    else
        pushBackLastToken();
    return node;
    }

Node* Parser::parsePostFixExpr()
    {
    Node* node = parsePrimaryExpr();
    auto t = nextToken();
    if (t.type == TokenType::DOT)
        {
        t = nextToken();
        if (t.type == TokenType::ID)
            {
            auto postfixExpr = (PostfixExpr*)createPostfix();
            postfixExpr->primExpr = node;
            postfixExpr->postfixId = t;
            return postfixExpr;
            }
        else
            pushBackLastToken();
        }
    else
        pushBackLastToken();
    return node;
    }

Node* Parser::parsePrimaryExpr()
    {
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
        if (FunctionDef::exists(t.stringValue))
            {
            return parseCallExpr(t);
            }
        else
            {
            PrimaryExpr* primExpr = createPrimary();
            primExpr->Id = t;
            return primExpr;
            }
        }
    else if (t.type == TokenType::PAR_OPEN)
        {
        PrimaryExpr* primExpr = createPrimary();//TODO: get rid of this.
        primExpr->addExpr = parseAddExpr();
        t = nextToken();
        //if (t.type != TokenType::PAR_CLOSE)
        //    primExpr.errorPos = 1;//todo
        return primExpr; //always return a 'wrapped' expression as a primExpr, not it's content, for analysis and warnings.
        }
    return createPrimary(); //error
    }

ConstExpr* Parser::parseConst(bool negative)
    {
    ConstExpr* constExpr = createConst();
    auto t = nextToken();
    t.numberValue = (negative ? -1 : 1) * t.numberValue;
    constExpr->constNumber = t;
    t = nextToken();
    if (t.type == TokenType::ID)
        {
        if (ids.count(t.stringValue) != 0)
            pushBackLastToken(); //a known id: assuming an implicit mult, here.
        else
            constExpr->unit = t; //no known id: assuming a unit.
        }
    else
        pushBackLastToken();

    return constExpr;
    }

CallExpr* Parser::parseCallExpr(Token functionName)
    {
    CallExpr* callExpr = createCall();
    callExpr->functionName = functionName;
    auto t = nextToken();
    if (t.type != TokenType::PAR_OPEN)
        {
        callExpr->error = Error(ErrorId::FUNC_NO_OPEN_PAR, functionName.stringValue.c_str());
        pushBackLastToken();
        return callExpr;
        }
    //loop arguments
    while (peekToken().type != TokenType::EOT)
        {
        if (peekToken().type == TokenType::SEMI_COLON)
            break; //allow parsing of next statement
        callExpr->arguments.push_back(parseAddExpr());
        auto t = nextToken();
        if (t.type == TokenType::PAR_CLOSE)
            {
            break;
            }
        if (t.type != TokenType::COMMA)
            {
            pushBackLastToken();
            break;
            }
        }

    return callExpr;
    }

ConstExpr* Parser::createConst() { ConstExpr* p = new ConstExpr; nodes.push_back(p); return p; }
AddExpr* Parser::createAdd()   { AddExpr* p = new AddExpr; nodes.push_back(p); return p; }
MultExpr* Parser::createMult() { MultExpr* p = new MultExpr; nodes.push_back(p); return p; }
PowerExpr* Parser::createPower() { PowerExpr* p = new PowerExpr; nodes.push_back(p); return p; }
PrimaryExpr* Parser::createPrimary() { PrimaryExpr* p = new PrimaryExpr; nodes.push_back(p); return p; }
PostfixExpr* Parser::createPostfix() { PostfixExpr* p = new PostfixExpr; nodes.push_back(p); return p; }
AssignExpr* Parser::createAssign() { AssignExpr* p = new AssignExpr; nodes.push_back(p); return p; }
Statement* Parser::createStatement() { Statement* p = new Statement; nodes.push_back(p); return p; }
CallExpr* Parser::createCall() { CallExpr* p = new CallExpr; nodes.push_back(p); return p; }

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
#ifndef __WASM__
    if (lastToken.type != TokenType::NULLPTR)
        throw new std::exception("Cannot push back a second token.");
#endif
    lastToken = currentToken;
    }
