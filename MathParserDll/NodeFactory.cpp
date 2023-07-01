#include "pch.hpp"
#include "NodeFactory.h"

NoneExpr* NodeFactory::createErrorExpr(Error error)
    { 
    NoneExpr* p = new NoneExpr(); 
    nodes.push_back(p); 
    p->error = error;
    return p; 
    }

NodeFactory::~NodeFactory() { for (auto node : nodes) delete node; }

FunctionDefExpr* NodeFactory::createFunctionDef() { FunctionDefExpr* p = new FunctionDefExpr(); nodes.push_back(p); return p; }
NoneExpr* NodeFactory::createNoneExpr() { NoneExpr* p = new NoneExpr(); nodes.push_back(p); return p; }
ConstExpr* NodeFactory::createConst(ConstType type) { ConstExpr* p = new ConstExpr(type); nodes.push_back(p); return p; }
BinaryOpExpr* NodeFactory::createBinaryOp() { BinaryOpExpr* p = new BinaryOpExpr; nodes.push_back(p); return p; }
UnaryOpExpr* NodeFactory::createUnaryOp() { UnaryOpExpr* p = new UnaryOpExpr; nodes.push_back(p); return p; }
IdExpr* NodeFactory::createIdExpr() { IdExpr* p = new IdExpr; nodes.push_back(p); return p; }
PostfixExpr* NodeFactory::createPostfix() { PostfixExpr* p = new PostfixExpr; nodes.push_back(p); return p; }
AssignExpr* NodeFactory::createAssign() { AssignExpr* p = new AssignExpr; nodes.push_back(p); return p; }
ListExpr* NodeFactory::createList() { ListExpr* p = new ListExpr; nodes.push_back(p); return p; }
Define* NodeFactory::createDefine() { Define* p = new Define; nodes.push_back(p); return p; }
Statement* NodeFactory::createStatement() { Statement* p = new Statement; nodes.push_back(p); return p; }
CallExpr* NodeFactory::createCall() { CallExpr* p = new CallExpr; nodes.push_back(p); return p; }
