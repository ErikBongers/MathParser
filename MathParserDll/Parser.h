#pragma once
#include "NodeFactory.h"
#include <memory>
#include "CodeBlock.h"
#include "PeekingTokenizer.h"

class Parser
    {
    private:
        PeekingTokenizer& tok;
        NodeFactory& nodeFactory;
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;
        TokenPos statementStartPos;
    public:
        CodeBlock& codeBlock;
        Parser(CodeBlock& codeBlock, NodeFactory& nodeFactory, PeekingTokenizer& tok);
        void parse();
    private:
        Statement* parseStatement();
        CodeBlock parseBlock();
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
        Node* reduceList(ListExpr* listExpr);

        Node* parseAbsOperator(const Token& currentToken);
        void parseEchosBetweenStatements(Statement* lastStmt);
        void parseEchoLines();
        bool match(TokenType tt);
        bool peek(TokenType tt) { return tok.peek().type == tt; }

    };

