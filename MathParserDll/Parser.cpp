#include "pch.h"
#include "Parser.h"
#include "Function.h"

Parser::Parser(const char* stream, FunctionDefs& functionDefs)
    : tok(stream), functionDefs(functionDefs) 
    {
    }
# define M_PIl          3.141592653589793238462643383279502884L
void Parser::parse()
    {
    ConstExpr* pConst = createConst(ValueType::NUMBER);
    pConst->value = Token(TokenType::NUMBER, Number(M_PIl, 0), tok.getLine(), tok.getLinePos());
    ids.emplace("pi", Variable{ Token(TokenType::ID, "pi", tok.getLine(), tok.getLinePos()), pConst});
    while (peekToken().type != TokenType::EOT)
        {
        auto stmt = parseStatement();
        statements.push_back(stmt);
        parseEchosBetweenStatements(stmt);
        }
    }

void Parser::parseEchosBetweenStatements(Statement* stmt)
    {
    if (peekToken(true).type == TokenType::ECHO_COMMENT_LINE
        || (echoTrailingComment && peekToken(true).type == TokenType::COMMENT_LINE))
        {
        auto t = tok.next(true);
        if (t.isFirstOnLine)
            {
            stmt = createStatement();
            stmt->comment_line = t;
            statements.push_back(stmt);
            }
        else
            {
            stmt->comment_line = t;
            }
        }
    echoTrailingComment = false;
    while (peekToken(true).type == TokenType::ECHO_COMMENT_LINE || peekToken(true).type == TokenType::COMMENT_LINE)
        {
        auto t = tok.next(true);
        if(t.type == TokenType::ECHO_COMMENT_LINE)
            {
            stmt = createStatement();
            stmt->comment_line = t;
            statements.push_back(stmt);
            }
        }
    }

Define* Parser::parseDefine()
    {
    auto t = peekToken();
    if (t.type == TokenType::DEFINE || t.type == TokenType::UNDEF)
        {
        auto define = createDefine();
        define->def = nextToken();//consume;
        return define;
        }
    return nullptr;
    }

Statement* Parser::parseStatement()
    {
    Statement* stmt = createStatement();
    stmt->node = parseDefine();
    if(stmt->node== nullptr)
        {
        stmt = parseStatementHeader(stmt);
        stmt->mute |= this->muteBlock;
        if(stmt->echo)
            stmt->text = tok.getText(statementStartPos, tok.getPos());
        }
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
    if(peekToken().type == TokenType::ECHO_DOUBLE)
        {
        nextToken(); //consume ECHO
        stmt->echo = true;
        echoTrailingComment = true;
        return parseStatementHeader(stmt);
        }
    if(peekToken().type == TokenType::ECHO_COMMENT_LINE)
        {
        auto t = nextToken(); //consume ECHO
        stmt = createStatement();
        stmt->comment_line = t;
        return stmt;
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
    if(peekToken().type == TokenType::ECHO_START)
        {
        nextToken(); //consume ECHO
        this->echoBlock = true;
        return parseStatementHeader(stmt);
        }
    if(peekToken().type == TokenType::ECHO_END)
        {
        nextToken(); //consume ECHO
        this->echoBlock = false;
        return parseStatementHeader(stmt);
        }
    tok.skipWhiteSpace();
    statementStartPos = tok.getPos();
    return parseStatementBody(stmt);
    }

Statement* Parser::parseStatementBody(Statement* stmt)
    {
    stmt->node = parseAssignExpr();
    if (stmt->node == nullptr)
        stmt->node = parseAddExpr();
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
        }
    //else
    //   don't  pushBackLastToken --> risk of dead loop
    if(echoBlock)
        stmt->echo = true;
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
            auto list = parseListExpr();
            if(list.size() == 1)
                assign->expr = list[0];
            else
                {
                auto listExpr = createList();
                listExpr->list = list;
                assign->expr = listExpr;
                }

            ids.emplace(assign->Id.stringValue, Variable{ assign->Id, assign->expr });
            return assign;
            }
        else if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV || t.type == TokenType::EQ_UNIT)
            {
            nextToken();//consume the EQ
            AssignExpr* assign = createAssign();
            assign->Id = id;
            //prepare components to fabricate the new add or mult expression.
            IdExpr* idExpr = createIdExpr();
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
            if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV)
                {
                BinaryOpExpr* binOpExpr = createBinaryOp();
                binOpExpr->n1 = idExpr;
                binOpExpr->op = Token(oper, tok.getLine(), tok.getLinePos());
                binOpExpr->n2 = parseAddExpr();
                assign->expr = binOpExpr;
                }
            else if (t.type == TokenType::EQ_UNIT)
                {
                PostfixExpr* pfix = createPostfix();
                pfix->expr = idExpr;
                assign->Id = id;
                assign->expr = pfix;
                auto t = nextToken();
                if (t.type == TokenType::ID)
                    {
                    pfix->postfixId = t;
                    }
                else
                    { //valid syntax: clear the unit, if any.
                    pfix->postfixId = Token::Null();
                    pushBackLastToken();
                    }
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
        BinaryOpExpr* addExpr = createBinaryOp();
        addExpr->n1 = node;
        addExpr->op = t;
        addExpr->n2 = parseMultExpr();
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
        BinaryOpExpr* multExpr = createBinaryOp();
        multExpr->n1 = node;
        multExpr->op = t;
        multExpr->n2 = parsePowerExpr();
        node = multExpr;
        //give warning if expr of form a/2b:
        if (multExpr->op.type == TokenType::DIV)
            {
            if (multExpr->n2->is(NodeType::BINARYOPEXPR))
                {
                BinaryOpExpr* n2 = static_cast<BinaryOpExpr*>(multExpr->n2);
                if (n2->implicitMult)
                    {
                    multExpr->error = Error(ErrorId::W_DIV_IMPL_MULT, tok.getLine(), tok.getLinePos());
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
        BinaryOpExpr* powerExpr = createBinaryOp();
        powerExpr->n1 = node;
        powerExpr->op = t;
        powerExpr->n2 = parsePowerExpr(); //right associative!
        node = powerExpr;
        if ((powerExpr->n1->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n1)->implicitMult)
            || (powerExpr->n2->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n2)->implicitMult))
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
    Node* n1 = parseUnaryExpr();
    auto t = nextToken();
    while ((t.type == TokenType::ID
            || t.type == TokenType::NUMBER)
           || t.type == TokenType::PAR_OPEN)
        {
        pushBackLastToken();
        auto m = createBinaryOp();
        m->n1 = n1;
        m->op = Token(TokenType::MULT, '*', tok.getLine(), tok.getLinePos());
        m->n2 = parsePostFixExpr();
        m->implicitMult = true;
        n1 = m;
        t = nextToken();
        }
    pushBackLastToken();
    return n1;
    }

Node* Parser::parseUnaryExpr()
    {
    auto t = peekToken();
    if (t.type == TokenType::MIN)
        {
        nextToken();
        auto node = createUnaryOp();
        node->op = t;
        node->n = parsePostFixExpr();
        return node;
        }
    return parsePostFixExpr();
    }

Node* Parser::parsePostFixExpr()
    {
    Node* node = parseUnitExpr();
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
        auto postfixExpr = createPostfix();
        postfixExpr->expr = node;
        node = postfixExpr;
        if (t.type == TokenType::ID)
            {
            postfixExpr->postfixId = t;
            }
        else
            { //valid syntax: clear the unit, if any.
            postfixExpr->postfixId = Token::Null();
            pushBackLastToken();
            }
        }
    else if (t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        if (node->type != NodeType::IDEXPR)
            {
            node->error = Error(ErrorId::VAR_EXPECTED, tok.getLine(), tok.getLinePos());
            return node;
            }

        IdExpr* idExpr = (IdExpr*)node;
        CallExpr* callExpr = createCall();
        callExpr->arguments.push_back(node);
        callExpr->functionName = Token(TokenType::ID, (t.type == TokenType::INC ? "_ inc" : "_ dec"), tok.getLine(), tok.getLinePos());

        AssignExpr* assignExpr = createAssign();
        assignExpr->Id = idExpr->Id;
        assignExpr->expr = callExpr;
        node = assignExpr;
        }
    return node;
    }

Node* Parser::parseUnitExpr()
    {
    Node* node = parsePrimaryExpr();
    auto t = nextToken();
    if (t.type == TokenType::ID)
        {
        if (ids.count(t.stringValue) != 0)
            pushBackLastToken(); //a known id: assuming an implicit mult, here.
        else
            node->unit = t.stringValue; //no known id: assuming a unit.
        }
    else
        pushBackLastToken();
    return node;
    }

Node* Parser::parsePrimaryExpr()
    {
    auto t = nextToken();
    switch (t.type)
        {
        case TokenType::NUMBER:
            return parseNumber(false);
        case TokenType::MIN:
            if (peekToken().type == TokenType::NUMBER)
                {
                nextToken();
                return parseNumber(true);
                }
            //else error?
            break;
        case TokenType::ID:
            if (functionDefs.exists(t.stringValue))
                {
                return parseCallExpr(t);
                }
            else
                {
                IdExpr* idExpr = createIdExpr();
                idExpr->Id = t;
                return idExpr;
                }
        case TokenType::PAR_OPEN:
            {
            auto addExpr = parseAddExpr();
            t = nextToken();
            //if (t.type != TokenType::PAR_CLOSE)
            //    primExpr.errorPos = 1;//todo
            return addExpr;
            }
        case TokenType::PIPE:
            return parseAbsOperator();
        case TokenType::QUOTED_STR:
            {
            auto constExpr = createConst(ValueType::TIMEPOINT);
            constExpr->value = t;
            return constExpr;
            }
        default: 
            pushBackLastToken();
            break;
        }
    return createNoneExpr(); //error
    }

Node* Parser::parseAbsOperator()
    {
    auto addExpr = parseAddExpr();
    CallExpr* callExpr = createCall();
    callExpr->arguments.push_back(addExpr);
    callExpr->functionName = Token(TokenType::ID, "abs", tok.getLine(), tok.getLinePos());
    auto t = nextToken();
    //if (t.type != TokenType::PAR_CLOSE)
    //    callExpr.errorPos = 1;//todo
    return callExpr;
    }

ConstExpr* Parser::parseNumber(bool negative)
    {
    auto constExpr = createConst(ValueType::NUMBER);
    currentToken.numberValue.significand = (negative ? -1 : 1) * currentToken.numberValue.significand;
    constExpr->value = currentToken;
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
    callExpr->arguments = parseListExpr();
    t = nextToken();
    if (t.type != TokenType::PAR_CLOSE)
        pushBackLastToken();
    return callExpr;
    }

std::vector<Node*> Parser::parseListExpr()
    {
    std::vector<Node*> list;
    while (true)
        {
        //if (peekToken().type == TokenType::SEMI_COLON)
        //    break; //allow parsing of next statement
        auto expr = parseAddExpr();
        if(expr->is(NodeType::NONE))
            break;
        list.push_back(expr);
        auto t = nextToken();
        if (t.type == TokenType::PAR_CLOSE)
            {
            pushBackLastToken();
            break;
            }
        if (t.type != TokenType::COMMA)
            {
            pushBackLastToken();
            break;
            }
        }
    return list;
    }


Node* Parser::createNoneExpr() { Node* p = new Node(NodeType::NONE); nodes.push_back(p); return p; }
ConstExpr* Parser::createConst(ValueType type) { ConstExpr* p = new ConstExpr(type); nodes.push_back(p); return p; }
BinaryOpExpr* Parser::createBinaryOp() { BinaryOpExpr* p = new BinaryOpExpr; nodes.push_back(p); return p; }
UnaryOpExpr* Parser::createUnaryOp() { UnaryOpExpr* p = new UnaryOpExpr; nodes.push_back(p); return p; }
IdExpr* Parser::createIdExpr() { IdExpr* p = new IdExpr; nodes.push_back(p); return p; }
PostfixExpr* Parser::createPostfix() { PostfixExpr* p = new PostfixExpr; nodes.push_back(p); return p; }
AssignExpr* Parser::createAssign() { AssignExpr* p = new AssignExpr; nodes.push_back(p); return p; }
ListExpr* Parser::createList() { ListExpr* p = new ListExpr; nodes.push_back(p); return p; }
Define* Parser::createDefine() { Define* p = new Define; nodes.push_back(p); return p; }
Statement* Parser::createStatement() { Statement* p = new Statement; nodes.push_back(p); return p; }
CallExpr* Parser::createCall() { CallExpr* p = new CallExpr; nodes.push_back(p); return p; }

Token Parser::peekToken(bool includeEchoComment)
    {
    if (lastToken.type != TokenType::NULLPTR)
        return lastToken;
    else
        return tok.peek(includeEchoComment);
    }

Token Parser::nextToken(bool includeEchoComment)
    {
    if (lastToken.type != TokenType::NULLPTR)
        {
        currentToken = lastToken;
        lastToken = Token::Null();
        }
    else
        {
        currentToken = tok.next(includeEchoComment);
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
