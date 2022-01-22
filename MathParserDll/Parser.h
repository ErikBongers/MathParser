#pragma once
#include "Unit.h"
#include "Error.h"
#include "Tokenizer.h"
#include <vector>

enum class NodeType {CONSTEXPR, PRIMARYEXPR, CALLEXPR, MULTEXPR, ADDEXPR, POWEREXPR, ASSIGNMENT, STATEMENT};
class Parser;

class Node
    {
    protected:
        Node(NodeType type) : type(type) {}
    public:
        NodeType type;
        Error error;
        Unit unit;
        bool is(NodeType type) { return this->type == type; }
    private:
        Node() {}
        friend class Parser;
    };

class ConstExpr : public Node
    {
    public:
        Token constNumber;
    private:
        ConstExpr() : Node(NodeType::CONSTEXPR) {};
        friend class Parser;
    };

class PrimaryExpr : public Node
    {
    public:
        Token Id;
        Node* addExpr = nullptr;
        Node* callExpr = nullptr;
    private:
        PrimaryExpr() : Node(NodeType::PRIMARYEXPR) {}
        friend class Parser;
    };

class CallExpr : public Node
    {
    public:
        Token functionName;
        std::vector<Node*> arguments;
    private:
        CallExpr() : Node(NodeType::CALLEXPR) {}
        friend class Parser;
    };

class MultExpr : public Node
    {
    public:
        Node* m1 = nullptr;
        Token op;
        Node* m2 = nullptr;
        bool implicitMult = false;
    private:
        MultExpr() : Node(NodeType::MULTEXPR) {}
        friend class Parser;
    };

class PowerExpr : public Node
    {
    public:
        Node* p1 = nullptr;
        Node* p2 = nullptr;
    private:
        PowerExpr() : Node(NodeType::POWEREXPR) {}
        friend class Parser;
    };

class AddExpr : public Node
    {
    public:
        Node* a1 = nullptr;
        Token op;
        Node* a2 = nullptr;
    private:
        AddExpr() : Node(NodeType::ADDEXPR) {}
        friend class Parser;
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* addExpr = nullptr;
    private:
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class Parser;
    };

class Statement : public Node
    {
    public:
        Node* assignExpr = nullptr;
        Node* addExpr = nullptr;
    private:
        Statement() : Node(NodeType::STATEMENT) {}
        friend class Parser;
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
        ~Parser() { for (auto node : nodes) delete node; }
        ConstExpr* createConst();
        AddExpr* createAdd();
        MultExpr* createMult();
        PowerExpr* createPower();
        PrimaryExpr* createPrimary();
        AssignExpr* createAssign();
        CallExpr* createCall();
        Statement* createStatement();
    private:
        std::vector<Node*> nodes;
        Token currentToken = Token(TokenType::NULLPTR);
        Token lastToken = Token(TokenType::NULLPTR);

        Token peekToken();
        Token nextToken();
        void pushBackLastToken();
    };

