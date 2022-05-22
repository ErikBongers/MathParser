#pragma once
#include "NodeFactory.h"

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
        Scope* scope;
        std::vector<Statement*> statements;

        Parser(const char* stream, char sourceIndex, Scope* scope);
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
        std::vector<Node*> parseListExpr();

        Node* parseAbsOperator(const Token& currentToken);
        void parseEchosBetweenStatements(Statement* lastStmt);
        void parseEchoLines();
        bool match(TokenType tt);
        bool peek(TokenType tt) { return tok.peek().type == tt; }

    };

