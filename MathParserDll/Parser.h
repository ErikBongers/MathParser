#pragma once
#include "NodeFactory.h"
#include <memory>
#include "CodeBlock.h"

class Parser
    {
    private:
        Tokenizer tok;
        NodeFactory nodeFactory;
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;
        unsigned int statementStartPos = 0;
    public:
        CodeBlock codeBlock;
        Parser(const char* stream, char sourceIndex, std::unique_ptr<Scope>&& scope);
        void parse();
    private:
        Statement* parseStatement();
        Define* parseDefine();
        Statement* parseStatementHeader(Statement* stmt);
        Statement* parseExprStatement(Statement* stmt);
        Node* parseFunctionDef();
        Node* parseAssignExpr();
        Node* parseAddExpr();
        Node* parseMultExpr();
        Node* parsePowerExpr();
        Node* parseUnaryExpr();
        Node* parseImplicitMult();
        Node* parseUnitExpr();
        Node* parsePostFixExpr();
        Node* parseOnePostFix(Node* node);
        Node* parsePrimaryExpr();
        ConstExpr* parseNumber(Token currentToken, bool negative);
        CallExpr* parseCallExpr(Token functionName);
        ListExpr* parseListExpr();

        Node* parseAbsOperator(const Token& currentToken);
        void parseEchosBetweenStatements(Statement* lastStmt);
        void parseEchoLines();
        bool match(TokenType tt);
        bool peek(TokenType tt) { return tok.peek().type == tt; }

    };

