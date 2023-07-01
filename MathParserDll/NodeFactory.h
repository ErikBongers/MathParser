#pragma once
#include "Nodes.h"

//Serves as sort of memory allocation Arena.
class NodeFactory
    {
    private:
        std::vector<Node*> nodes;
    public:
        ~NodeFactory();
        FunctionDefExpr* createFunctionDef();
        NoneExpr* createErrorExpr(Error error);
        NoneExpr* createNoneExpr();
        ConstExpr* createConst(ConstType type);
        BinaryOpExpr* createBinaryOp();
        UnaryOpExpr* createUnaryOp();
        IdExpr* createIdExpr();
        PostfixExpr* createPostfix();
        AssignExpr* createAssign();
        ListExpr* createList();
        Define* createDefine();
        CallExpr* createCall();
        Statement* createStatement();
    };