#include "pch.h"
#include "Parser.h"
#include "Function.h"

void Parser::parse()
    {
    ConstExpr* pConst = createConst();
    pConst->constNumber = Token(TokenType::NUMBER, M_PI, tok.getLine(), tok.getLinePos());
    ids.emplace("pi", Variable{ Token(TokenType::ID, "pi", tok.getLine(), tok.getLinePos()), pConst});
    while (peekToken().type != TokenType::EOT)
        {
        statements.push_back(parseStatement());
        }
    }

Statement* Parser::parseStatement()
    {
    Statement* stmt = createStatement();
    stmt = parseStatementHeader(stmt);
    stmt->mute |= this->muteBlock;
    return stmt;
    }

Statement* Parser::parseStatementHeader(Statement* stmt)
    {
    if(peekToken().type == TokenType::ECHO)
        {
        nextToken(); //consume ECHO
        stmt->echo = true;
        return parseStatementHeader(stmt);
        }
    if(peekToken().type == TokenType::MUTE_LINE)
        {
        nextToken(); //consume MUTE
        stmt->mute = true;
        return parseStatementHeader(stmt);
        }
    if(peekToken().type == TokenType::MUTE_START)
        {
        nextToken(); //consume MUTE
        this->muteBlock = true;
        return parseStatementHeader(stmt);
        }
    if(peekToken().type == TokenType::MUTE_END)
        {
        nextToken(); //consume MUTE
        this->muteBlock = false;
        return parseStatementHeader(stmt);
        }
    tok.currentStatement.clear();//remove all the statement-header stuff and new lines between statements.
    return parseStatementBody(stmt);
    }

Statement* Parser::parseStatementBody(Statement* stmt)
    {
    stmt->assignExpr = parseAssignExpr();
    if (stmt->assignExpr == nullptr)
        stmt->addExpr = parseAddExpr();
    nextToken();
    if (currentToken.type == TokenType::SEMI_COLON)
        { 
        if(stmt->echo)
            {
            stmt->text = currentToken.stringValue;
            if(stmt->text.starts_with("\r\n"))
                stmt->text.erase(0, 2);
            else if(stmt->text.starts_with("\n"))
                stmt->text.erase(0, 1);
            }
        stmt->comment_lines = tok.comment_lines;
        tok.comment_lines.clear();
        }
    else
        pushBackLastToken();//continue, although what follows is an error
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
                default:break;
                }

            //build expression
            if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN)
                {
                AddExpr* addExpr = createAdd();
                addExpr->a1 = idExpr;
                addExpr->op = Token(oper, tok.getLine(), tok.getLinePos());
                addExpr->a2 = parseAddExpr();
                assign->expr = addExpr;
                }
            else
                {
                MultExpr* multExpr = createMult();
                multExpr->m1 = idExpr;
                multExpr->op = Token(oper, tok.getLine(), tok.getLinePos());
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
    Node* node = parseMultExpr();
    auto t = nextToken();
    while (t.type == TokenType::PLUS || t.type == TokenType::MIN)
        {
        AddExpr* addExpr = createAdd();
        addExpr->a1 = node;
        addExpr->op = t;
        addExpr->a2 = parseMultExpr();
        node = addExpr;
        t = nextToken();
        }
    pushBackLastToken();
    return node;
    }

Node* Parser::parseMultExpr()
    {
    Node* node = parsePowerExpr();
    auto t = nextToken();
    while (t.type == TokenType::MULT || t.type == TokenType::DIV)
        {
        MultExpr* multExpr = createMult();
        multExpr->m1 = node;
        multExpr->op = t;
        multExpr->m2 = parsePowerExpr();
        node = multExpr;
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
                        div->error = Error(ErrorId::W_DIV_IMPL_MULT, tok.getLine(), tok.getLinePos());
                        }
                    }
                }
            }
        t = nextToken();
        }
    pushBackLastToken();
    return node;
    }

Node* Parser::parsePowerExpr()
    {
    Node* node = parseImplicitMult();
    auto t = nextToken();
    while (t.type == TokenType::POWER)
        {
        PowerExpr* powerExpr = createPower();
        powerExpr->p1 = node;
        powerExpr->p2 = parsePowerExpr(); //right associative!
        node = powerExpr;
        if ((powerExpr->p1->is(NodeType::MULTEXPR) && static_cast<MultExpr*>(powerExpr->p1)->implicitMult)
            || (powerExpr->p2->is(NodeType::MULTEXPR) && static_cast<MultExpr*>(powerExpr->p2)->implicitMult))
            {
            powerExpr->error = Error(ErrorId::W_POW_IMPL_MULT, tok.getLine(), tok.getLinePos());
            }
        t = nextToken();
        }
    pushBackLastToken();
    return node;
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
        m->op = Token(TokenType::MULT, '*', tok.getLine(), tok.getLinePos());
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
    while (t.type == TokenType::DOT || t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        node = parseOnePostFix(node, t);
        t = nextToken();
        }
    pushBackLastToken();
    return node;
    }

Node* Parser::parseOnePostFix(Node* node, Token t)
    {
    if (t.type == TokenType::DOT)
        {
        t = nextToken();
        if (t.type == TokenType::ID)
            {
            auto postfixExpr = (PostfixExpr*)createPostfix();
            postfixExpr->primExpr = node;
            postfixExpr->postfixId = t;
            node = postfixExpr;
            }
        else
            { //valid syntax: clear the unit, if any.
            node->unit = Unit::CLEAR();
            pushBackLastToken();
            }
        }
    else if (t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        if (node->type != NodeType::PRIMARYEXPR)
            {
            node->error = Error(ErrorId::VAR_EXPECTED, tok.getLine(), tok.getLinePos());
            return node;
            }

        PrimaryExpr* idExpr = (PrimaryExpr*)node;
        CallExpr* callExpr = createCall();
        callExpr->arguments.push_back(node);
        callExpr->functionName = Token(TokenType::ID, (t.type == TokenType::INC ? "inc" : "dec"), tok.getLine(), tok.getLinePos());

        AssignExpr* assignExpr = createAssign();
        assignExpr->Id = idExpr->Id;
        assignExpr->expr = callExpr;
        node = assignExpr;
        }
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
        PrimaryExpr* primExpr = createPrimary();//TODO: get rid of this? See comment below...
        primExpr->addExpr = parseAddExpr();
        t = nextToken();
        //if (t.type != TokenType::PAR_CLOSE)
        //    primExpr.errorPos = 1;//todo
        return primExpr; //always return a 'wrapped' expression as a primExpr, not it's content, for analysis and warnings.
        }
    else if (t.type == TokenType::PIPE)
        {
        auto addExpr = parseAddExpr();
        CallExpr* callExpr = createCall();
        callExpr->arguments.push_back(addExpr);
        callExpr->functionName = Token(TokenType::ID, "abs", tok.getLine(), tok.getLinePos());
        t = nextToken();
        //if (t.type != TokenType::PAR_CLOSE)
        //    callExpr.errorPos = 1;//todo
        return callExpr;
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
        callExpr->error = Error(ErrorId::FUNC_NO_OPEN_PAR, tok.getLine(), tok.getLinePos(), functionName.stringValue.c_str());
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
        lastToken = Token::Null();
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
