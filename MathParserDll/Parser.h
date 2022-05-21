#pragma once
#include "Nodes.h"

class Parser
    {
    private:
        Tokenizer tok;
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;
        std::vector<Node*> nodes;
        unsigned int statementStartPos = 0;
    public:
        Scope* scope;
        std::vector<Statement*> statements;

        Parser(const char* stream, char sourceIndex, Scope* scope);
        ~Parser() { for (auto node : nodes) delete node; }
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

        FunctionDefExpr* createFunctionDef();
        NoneExpr* createErrorExpr(Error error);
        NoneExpr* createNoneExpr();
        ConstExpr* createConst(ValueType type);
        BinaryOpExpr* createBinaryOp();
        UnaryOpExpr* createUnaryOp();
        IdExpr* createIdExpr();
        PostfixExpr* createPostfix();
        AssignExpr* createAssign();
        ListExpr* createList();
        Define* createDefine();
        CallExpr* createCall();
        Statement* createStatement();

        Node* parseAbsOperator(const Token& currentToken);
        void parseEchosBetweenStatements(Statement* lastStmt);
        void parseEchoLines();
        bool match(TokenType tt);
        bool peek(TokenType tt) { return tok.peek().type == tt; }

    };

