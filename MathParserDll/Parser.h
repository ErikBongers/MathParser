#pragma once
#include "Error.h"
#include "Tokenizer.h"
#include <vector>

enum class NodeType {NULL_, CONSTEXPR, PRIMARYEXPR, CALLEXPR, MULTEXPR, ADDEXPR, POWEREXPR, ASSIGNMENT, STATEMENT};
class Node
    {
    protected:
        Node(NodeType type) : type(type) {}
    public:
        NodeType type = NodeType::NULL_;
        Error error;
        Node() {}
        bool isNull() { return type == NodeType::NULL_; }
        bool is(NodeType type) { return this->type == type; }
    };

class ConstExpr : public Node
    {
    public:
        Token constNumber;
        Token unit;
        ConstExpr() : Node(NodeType::CONSTEXPR) {};
        ConstExpr(Token constNumber) : Node(NodeType::CONSTEXPR) , constNumber(constNumber) {};
    };

class PrimaryExpr : public Node
    {
    public:
        Token Id;
        Node* addExpr = nullptr;
        Node* callExpr = nullptr;
        PrimaryExpr() : Node(NodeType::PRIMARYEXPR) {}
    };

class CallExpr : public Node
    {
    public:
        Token functionName;
        Node* argument = nullptr; //TODO: make argument list
        CallExpr() : Node(NodeType::CALLEXPR) {}
    };

class MultExpr : public Node
    {
    public:
        Node* m1 = nullptr;
        Token op;
        Node* m2 = nullptr;
        bool implicitMult = false;
        MultExpr() : Node(NodeType::MULTEXPR) {}
    };

class PowerExpr : public Node
    {
    public:
        Node* p1 = nullptr;
        Node* p2 = nullptr;
        PowerExpr() : Node(NodeType::POWEREXPR) {}
    };

class AddExpr : public Node
    {
    public:
        Node* a1 = nullptr;
        Token op;
        Node* a2 = nullptr;
        AddExpr() : Node(NodeType::ADDEXPR) {}
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* addExpr = nullptr;
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
    };

class Statement : public Node
    {
    public:
        Node* assignExpr = nullptr;
        Node* addExpr = nullptr;
        Statement() : Node(NodeType::STATEMENT) {}
    };

class Variable
    {
    public:
        Token name;
        Node* addExpr = nullptr;
    };


class Parser
    {
    private:
        Tokenizer tok;
    public:
        std::map<std::string, Variable> ids;
        std::vector<Statement*> statements;
        
        Parser(const char* stream) : tok(stream) { }
        void parse();
        Statement* parseStatement();
        Node* parseAssignExpr();
        Node* parseAddExpr();
        Node* parseMultExpr();
        Node* parsePowerExpr();
        Node* parseImplicitMult();
        Node* parsePrimaryExpr();
        ConstExpr* parseConst(bool negative);
        CallExpr* parseCallExpr(Token functionName);
    private:
        Token currentToken = Token(TokenType::NULLPTR);
        Token lastToken = Token(TokenType::NULLPTR);

        Token peekToken();
        Token nextToken();
        void pushBackLastToken();
    };

