#include "pch.hpp"
#include "Parser.h"
#include "Function.h"
#include "Globals.h"
#include "Variable.h"
#include "Scope.h"

Parser::Parser(const char* stream, char sourceIndex, std::unique_ptr<Scope>&& scope)
    : tok(stream, sourceIndex), codeBlock(std::move(scope))
    {
    codeBlock._stream = stream;
    }

void Parser::parse()
    {
    parseEchoLines();
    while (!peek(TokenType::EOT))
        {
        auto stmt = parseStatement();
        codeBlock.statements.push_back(stmt);
        parseEchosBetweenStatements(stmt);
        }
    }

void Parser::parseEchosBetweenStatements(Statement* lastStmt)
    {
    tok.tokenizeComments(true);
    if (peek(TokenType::ECHO_COMMENT_LINE)
        || (echoTrailingComment && peek(TokenType::COMMENT_LINE)))
        {
        auto t = tok.next();
        if (t.isFirstOnLine)
            {
            auto stmt = nodeFactory.createStatement();
            stmt->comment_line = t;
            codeBlock.statements.push_back(stmt);
            }
        else
            {
            lastStmt->comment_line = t;
            }
        }
    echoTrailingComment = false;
    parseEchoLines();
    }

void Parser::parseEchoLines()
    {
    tok.tokenizeComments(true);
    while (peek(TokenType::ECHO_COMMENT_LINE) || peek(TokenType::COMMENT_LINE))
        {
        auto t = tok.next();
        if(t.type == TokenType::ECHO_COMMENT_LINE)
            {
            auto stmt = nodeFactory.createStatement();
            stmt->comment_line = t;
            codeBlock.statements.push_back(stmt);
            }
        }
    tok.tokenizeComments(false);
    }

Define* Parser::parseDefine()
    {
    auto t = tok.peek();
    if (t.type == TokenType::DEFINE || t.type == TokenType::UNDEF)
        {
        tok.next();
        auto define = nodeFactory.createDefine();
        define->undef = (t.type == TokenType::UNDEF);
        define->def_undef = t;
        tok.tokenizeNewlines(true);
        while (!peek(TokenType::EOT))
            {
            Token t = tok.peek();
            if (t.type == TokenType::NEWLINE || t.type == TokenType::SEMI_COLON)
                {
                tok.next();
                break;
                }
            define->defs.push_back(tok.next());
            }
        tok.tokenizeNewlines(false);
        return define;
        }
    return nullptr;
    }

Statement* Parser::parseStatement()
    {
    Statement* stmt = nodeFactory.createStatement();

    stmt->node = parseDefine();
    if(stmt->node != nullptr)
        return stmt;

    stmt->node = parseFunctionDef();
    if (stmt->node != nullptr)
        return stmt;

    stmt = parseStatementHeader(stmt);
    stmt->mute |= this->muteBlock;
    if(stmt->echo)
        stmt->text = tok.getText(statementStartPos, tok.getCurrentToken().range.start.cursorPos);
    return stmt;
    }

bool Parser::match(TokenType tt)
    {
    if (tok.peek().type != tt)
        return false;

    tok.next();
    return true;
    }

Node* Parser::parseFunctionDef()
    {
    if(!match(TokenType::FUNCTION))
        return nullptr;
    
    if (!peek(TokenType::ID))
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED_ID, tok.peek().range));
    auto id = tok.next();

    if (!match(TokenType::PAR_OPEN))
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED, tok.peek().range, "("));

    std::vector<std::string> paramDefs;
    while (tok.peek().type == TokenType::ID)
        {
        paramDefs.push_back(tok.getText(tok.next().range));
        if (match(TokenType::COMMA))
            continue;
        if(peek(TokenType::PAR_CLOSE))
           break;
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED, tok.peek().range, ",' or ')"));
        }

    if (!match(TokenType::PAR_CLOSE))
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED, tok.peek().range, ")"));

    if (!match(TokenType::CURL_OPEN))
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED, tok.peek().range, "{"));

    std::vector<Statement*> functionStmts;
    auto newScope = codeBlock.scope->copyForFunction();

    std::swap(newScope, codeBlock.scope);
    while (!peek(TokenType::CURL_CLOSE) && !peek(TokenType::EOT))
        {
        auto stmt = parseStatement();
        if(stmt == nullptr)
            break;
        functionStmts.push_back(stmt);
        }
    std::swap(codeBlock.scope, newScope);

    if (!match(TokenType::CURL_CLOSE))
        return nodeFactory.createErrorExpr(Error(ErrorId::EXPECTED, tok.peek().range, "}"));
    auto funcDef = nodeFactory.createFunctionDef();
    funcDef->id = tok.getText(id.range);
    funcDef->idRange = id.range;
    funcDef->params = paramDefs;
    funcDef->r = id.range;
    for(auto& stmt : functionStmts)
        funcDef->r += stmt->range();

    codeBlock.scope->AddLocalFunction(*funcDef, CodeBlock(std::move(newScope), std::move(functionStmts), codeBlock._stream));
    return funcDef;
    }

Statement* Parser::parseStatementHeader(Statement* stmt)
    {
    if(match(TokenType::EXCLAM))
        {
        stmt->echo = true;
        return parseStatementHeader(stmt);
        }
    if(match(TokenType::ECHO_DOUBLE))
        {
        stmt->echo = true;
        echoTrailingComment = true;
        return parseStatementHeader(stmt);
        }
    if(peek(TokenType::ECHO_COMMENT_LINE))
        {
        auto t = tok.next(); //consume ECHO
        stmt = nodeFactory.createStatement();
        stmt->comment_line = t;
        return stmt;
        }
    if(match(TokenType::MUTE_LINE))
        {
        stmt->mute = true;
        return parseStatementHeader(stmt);
        }
    if(match(TokenType::MUTE_START))
        {
        this->muteBlock = true;
        return parseStatementHeader(stmt);
        }
    if(match(TokenType::MUTE_END))
        {
        this->muteBlock = false;
        return parseStatementHeader(stmt);
        }
    if(match(TokenType::ECHO_START))
        {
        this->echoBlock = true;
        return parseStatementHeader(stmt);
        }
    if(match(TokenType::ECHO_END))
        {
        this->echoBlock = false;
        return parseStatementHeader(stmt);
        }
    statementStartPos = tok.getPos();
    return parseExprStatement(stmt);
    }

Statement* Parser::parseExprStatement(Statement* stmt)
    {
    tok.tokenizeComments(false);
    stmt->node = parseAssignExpr();
    if (stmt->node == nullptr)
        stmt->node = parseAddExpr();
    auto t = tok.peek();
    if (t.type == TokenType::SEMI_COLON)
        {
        tok.next(); //consume
        if(stmt->echo)
            {
            stmt->text = tok.getText(t.range);
            if(stmt->text.starts_with("\r\n"))
                stmt->text.erase(0, 2);
            else if(stmt->text.starts_with("\n"))
                stmt->text.erase(0, 1);
            }
        }
    else if (t.type == TokenType::EOT)
        {
        //ignore
        }
    else
        {
        tok.next();
        stmt->error = Error(ErrorId::EXPECTED, t.range, ";");
        }
    if(echoBlock)
        stmt->echo = true;
    return stmt;
    }

Node* Parser::parseAssignExpr()
    {
    auto t = tok.peek();
    if (t.type == TokenType::ID)
        {
        auto id = t;
        t = tok.peekSecond();
        if (t.type == TokenType::EQ)
            {
            tok.next();//consume ID
            tok.next();//consume EQ
            AssignExpr* assign = nodeFactory.createAssign();
            assign->Id = id;
            assign->expr = reduceList(parseListExpr());

            codeBlock.scope->emplaceVarDef(tok.getText(assign->Id.range), Variable{ assign->Id, assign->expr });
            return assign;
            }
        else if (t.type == TokenType::EQ_PLUS || t.type == TokenType::EQ_MIN || t.type == TokenType::EQ_MULT || t.type == TokenType::EQ_DIV || t.type == TokenType::EQ_UNIT)
            {
            tok.next(); //consume ID
            auto operToken = tok.next();//consume EQ
            AssignExpr* assign = nodeFactory.createAssign();
            assign->Id = id;
            //prepare components to fabricate the new add or mult expression.
            IdExpr* idExpr = nodeFactory.createIdExpr();
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
                BinaryOpExpr* binOpExpr = nodeFactory.createBinaryOp();
                binOpExpr->n1 = idExpr;
                binOpExpr->op = Token(oper, operToken.range.start, operToken.range.end);
                binOpExpr->n2 = parseAddExpr();
                assign->expr = binOpExpr;
                }
            else if (t.type == TokenType::EQ_UNIT)
                {
                PostfixExpr* pfix = nodeFactory.createPostfix();
                pfix->expr = idExpr;
                assign->Id = id;
                assign->expr = pfix;
                auto t = tok.peek();
                if (t.type == TokenType::ID)
                    {
                    tok.next();
                    pfix->postfixId = t;
                    }
                else
                    { //valid syntax: clear the unit, if any.
                    pfix->postfixId = Token::Null();
                    }
                }
            return assign;
            }
        else
            {
            return nullptr;
            }
        }
    else
        {
        return nullptr;
        }
    }

Node* Parser::parseAddExpr()
    {
    Node* node = parseMultExpr();
    auto t = tok.peek();
    while (t.type == TokenType::PLUS || t.type == TokenType::MIN)
        {
        tok.next();
        BinaryOpExpr* addExpr = nodeFactory.createBinaryOp();
        addExpr->n1 = node;
        addExpr->op = t;
        addExpr->n2 = parseMultExpr();
        node = addExpr;
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parseMultExpr()
    {
    Node* node = parsePowerExpr();
    auto t = tok.peek();
    while (t.type == TokenType::MULT || t.type == TokenType::DIV || t.type == TokenType::PERCENT || t.type == TokenType::MODULO)
        {
        tok.next();
        BinaryOpExpr* multExpr = nodeFactory.createBinaryOp();
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
                    multExpr->error = Error(ErrorId::W_DIV_IMPL_MULT, node->range());
                    }
                }
            }
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parsePowerExpr()
    {
    Node* node = parseImplicitMult();
    auto t = tok.peek();
    while (t.type == TokenType::POWER)
        {
        tok.next();
        BinaryOpExpr* powerExpr = nodeFactory.createBinaryOp();
        powerExpr->n1 = node;
        powerExpr->op = t;
        powerExpr->n2 = parsePowerExpr(); //right associative!
        node = powerExpr;
        if ((powerExpr->n1->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n1)->implicitMult)
            || (powerExpr->n2->is(NodeType::BINARYOPEXPR) && static_cast<BinaryOpExpr*>(powerExpr->n2)->implicitMult))
            {
            powerExpr->error = Error(ErrorId::W_POW_IMPL_MULT, node->range());
            }
        t = tok.peek();
        }
    return node;
    }

Node* Parser::reduceList(ListExpr* listExpr)
    {
    if(listExpr->list.size() == 1)
        return listExpr->list[0];
    return listExpr;
    }

Node* Parser::parseImplicitMult()
    {
    Node* n1 = parseUnaryExpr();
    auto t = tok.peek();
    while ((t.type == TokenType::ID
            || t.type == TokenType::NUMBER)
           || t.type == TokenType::PAR_OPEN)
        {
        //don't consume the token yet...
        auto m = nodeFactory.createBinaryOp();
        m->n1 = n1;
        m->op = Token(TokenType::MULT, 0, tok.peek().range.start);
        if(t.type == TokenType::PAR_OPEN)
            {
            m->n2 = reduceList(parseListExpr());
            }
        else
            m->n2 = parsePostFixExpr();
        m->implicitMult = true;
        n1 = m;
        t = tok.peek();
        }
    return n1;
    }

Node* Parser::parseUnaryExpr()
    {
    auto t = tok.peek();
    if (t.type == TokenType::MIN)
        {
        tok.next();
        auto node = nodeFactory.createUnaryOp();
        node->op = t;
        node->n = parsePostFixExpr();
        return node;
        }
    return parsePostFixExpr();
    }

Node* Parser::parsePostFixExpr()
    {
    Node* node = parseUnitExpr();
    auto t = tok.peek();
    while (t.type == TokenType::DOT || t.type == TokenType::INC || t.type == TokenType::DEC || t.type == TokenType::EXCLAM)
        {
        node = parseOnePostFix(node);
        t = tok.peek();
        }
    return node;
    }

Node* Parser::parseOnePostFix(Node* node)
    {
    auto t = tok.peek();
    if (t.type == TokenType::DOT)
        {
        tok.next();
        t = tok.peek();
        auto postfixExpr = nodeFactory.createPostfix();
        postfixExpr->expr = node;
        node = postfixExpr;
        if (t.type == TokenType::ID)
            {
            tok.next();
            postfixExpr->postfixId = t;
            }
        else
            { //a dot followed by nothing is valid syntax: clear the unit, if any.
            postfixExpr->postfixId = Token(TokenType::NULLPTR, 0, t.range.end);
            }
        }
    else if (t.type == TokenType::INC || t.type == TokenType::DEC)
        {
        auto operToken = tok.next();
        if (node->type != NodeType::IDEXPR)
            {
            node->error = Error(ErrorId::VAR_EXPECTED, node->range());
            return node;
            }

        IdExpr* idExpr = (IdExpr*)node;
        CallExpr* callExpr = nodeFactory.createCall();
        callExpr->arguments = nodeFactory.createList();
        callExpr->arguments->list.push_back(node);
        callExpr->functionName = t.type == TokenType::INC ? "_ inc" : "_ dec";
        callExpr->functionNameRange = operToken.range;

        AssignExpr* assignExpr = nodeFactory.createAssign();
        assignExpr->Id = idExpr->Id;
        assignExpr->expr = callExpr;
        node = assignExpr;
        }
    else if (t.type == TokenType::EXCLAM)
        {
        auto operToken = tok.next();
        auto callExpr = nodeFactory.createCall();
        callExpr->functionName = "factorial";
        callExpr->functionNameRange = operToken.range;
        callExpr->arguments = nodeFactory.createList();
        callExpr->arguments->list.push_back(node);
        return callExpr;
        }
    return node;
    }

Node* Parser::parseUnitExpr()
    {
    Node* node = parsePrimaryExpr();
    auto t = tok.peek();
    if (t.type == TokenType::ID && !codeBlock.scope->varDefExists(tok.getText(t.range)))
        {
        tok.next();
        node->unit = Unit(tok.getText(t.range), t.range); //no known id: assuming a unit.
        }
    return node;
    }

Node* Parser::parsePrimaryExpr()
    {
    auto t = tok.peek();
    switch (t.type)
        {
        case TokenType::NUMBER:
            return parseNumber(tok.next(), false);
        case TokenType::MIN:
            tok.next();
            if (tok.peek().type == TokenType::NUMBER)
                {
                return parseNumber(tok.next(), true);
                }
            //else error?
            break;
        case TokenType::ID:
            tok.next();
            if (codeBlock.scope->functionExists(tok.getText(t.range)))
                {
                return parseCallExpr(t);
                }
            else
                {
                IdExpr* idExpr = nodeFactory.createIdExpr();
                idExpr->Id = t;
                return idExpr;
                }
        case TokenType::PAR_OPEN:
            {
            tok.next();
            auto addExpr = reduceList(parseListExpr());
            if (!match(TokenType::PAR_CLOSE))
                {
                if(addExpr->error.id == ErrorId::NONE)
                    addExpr->error = Error(ErrorId::EXPECTED, addExpr->range(), ")");
                }
            if (addExpr->is(NodeType::BINARYOPEXPR))
                {
                auto binOp = (BinaryOpExpr*)addExpr;
                binOp->implicitMult = false;//parenthesis mean this flag is no longer applicable.
                }
            return addExpr;
            }
        case TokenType::PIPE:
            
            return parseAbsOperator(tok.next());
        case TokenType::QUOTED_STR:
            {
            tok.next();
            auto constExpr = nodeFactory.createConst(ValueType::TIMEPOINT);
            constExpr->value = t;
            return constExpr;
            }
        default: 
            break;
        }
    auto none = nodeFactory.createNoneExpr();
    none->token = t;
    return none;
    }

Node* Parser::parseAbsOperator(const Token& currentToken)
    {
    auto addExpr = parseAddExpr();
    CallExpr* callExpr = nodeFactory.createCall();
    callExpr->arguments = nodeFactory.createList();
    callExpr->arguments->list.push_back(addExpr);
    callExpr->functionName = "abs";
    callExpr->functionNameRange = currentToken.range;
    auto t = tok.peek();
    if (!match(TokenType::PIPE))
        {
        if(callExpr->error.id == ErrorId::NONE)
            callExpr->error = Error(ErrorId::EXPECTED, callExpr->range(), "|");
        }
    return callExpr;
    }

ConstExpr* Parser::parseNumber(Token currentToken, bool negative)
    {
    auto constExpr = nodeFactory.createConst(ValueType::NUMBER);
    currentToken.numberValue.significand = (negative ? -1 : 1) * currentToken.numberValue.significand;
    constExpr->value = currentToken;
    return constExpr;
    }

CallExpr* Parser::parseCallExpr(Token functionName)
    {
    CallExpr* callExpr = nodeFactory.createCall();
    callExpr->functionName = tok.getText(functionName.range);
    callExpr->functionNameRange = functionName.range;
    auto t = tok.peek();
    if (t.type != TokenType::PAR_OPEN)
        {
        callExpr->error = Error(ErrorId::FUNC_NO_OPEN_PAR, t.range, tok.getText(functionName.range).c_str());
        return callExpr;
        }
    tok.next();
    callExpr->arguments = parseListExpr();
    if (!match(TokenType::PAR_CLOSE))
        {
        if(callExpr->error.id == ErrorId::NONE)
            callExpr->error = Error(ErrorId::EXPECTED, callExpr->range(), ")");
        }
    return callExpr;
    }

ListExpr* Parser::parseListExpr()
    {
    Range range = tok.getCurrentToken().range;
    std::vector<Node*> list;
    while (true)
        {
        auto expr = parseAddExpr();
        if(expr->is(NodeType::NONE))
            break;
        list.push_back(expr);
        if (tok.peek().type != TokenType::COMMA)
            break;
        tok.next();
        }
    auto listExpr = nodeFactory.createList();
    listExpr->list = std::move(list);
    if (listExpr->list.size() == 0)
        {
        listExpr->error = Error(ErrorId::UNKNOWN_EXPR, range);
        }
    return listExpr;
    }

