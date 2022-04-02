#pragma once
#include "Unit.h"
#include "Error.h"
#include "Tokenizer.h"
#include <vector>
#include "Function.h"
#include "ValueType.h"

enum class NodeType {CONSTEXPR, POSTFIXEXPR, IDEXPR, CALLEXPR, BINARYOPEXPR, UNARYOPEXPR, ASSIGNMENT, STATEMENT, DEFINE, LIST, NONE};
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
        Node() : type(NodeType::CONSTEXPR) {}
        friend class Parser;
    };

class ConstExpr : public Node
    {
    public:
        ValueType type;
        Token value;
    private:
        ConstExpr(ValueType type) : Node(NodeType::CONSTEXPR), type(type) {};
        friend class Parser;
    };

class IdExpr : public Node
    {
    public:
        Token Id;
    private:
        IdExpr() : Node(NodeType::IDEXPR) {}
        friend class Parser;
    };

class PostfixExpr : public Node
    {
    public:
        Node* expr = nullptr;
        Token postfixId;
    private:
        PostfixExpr() : Node(NodeType::POSTFIXEXPR) {}
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

class BinaryOpExpr : public Node
    {
    public:
        Node* n1 = nullptr;
        Token op;
        Node* n2 = nullptr;
        bool implicitMult = false;
    private:
        BinaryOpExpr() : Node(NodeType::BINARYOPEXPR) {}
        friend class Parser;
    };

class UnaryOpExpr : public Node
    {
    public:
        Token op;
        Node* n = nullptr;
    private:
        UnaryOpExpr() : Node(NodeType::UNARYOPEXPR) {}
        friend class Parser;
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* expr = nullptr;
    private:
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class Parser;
    };

class ListExpr : public Node
    {
    public: 
        std::vector<Node*> list;
    private:
        ListExpr() : Node(NodeType::LIST) {}
        friend class Parser;
    };

class Define : public Node
    {
    public:
        Token def;
    private:
        Define() : Node(NodeType::DEFINE) {}
        friend class Parser;
    };

class Statement : public Node
    {
    public:
        Node* node = nullptr;
        std::string text;
        Token comment_line;
        bool mute = false; //mute output
        bool echo = false; //echo statement
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
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;
    public:
        FunctionDefs& functionDefs;
        std::map<std::string, Variable> ids;
        std::vector<Statement*> statements;

        Parser(const char* stream, FunctionDefs& functionDefs);
        ~Parser() { for (auto node : nodes) delete node; }
        void parse();
    private:
        Statement* parseStatement();
        Define* parseDefine();
        Statement* parseStatementHeader(Statement* stmt);
        Statement* parseStatementBody(Statement* stmt);
        Node* parseAssignExpr();
        Node* parseAddExpr();
        Node* parseMultExpr();
        Node* parsePowerExpr();
        Node* parseUnaryExpr();
        Node* parseImplicitMult();
        Node* parseUnitExpr();
        Node* parsePostFixExpr();
        Node* parseOnePostFix(Node* node, Token t);
        Node* parsePrimaryExpr();
        ConstExpr* parseNumber(bool negative);
        CallExpr* parseCallExpr(Token functionName);
        std::vector<Node*> parseListExpr();
        ConstExpr* createConst(ValueType type);
        BinaryOpExpr* createBinaryOp();
        UnaryOpExpr* createUnaryOp();
        IdExpr* createIdExpr();
        Node* createNoneExpr();
        PostfixExpr* createPostfix();
        AssignExpr* createAssign();
        ListExpr* createList();
        Define* createDefine();
        CallExpr* createCall();
        Statement* createStatement();
        std::vector<Node*> nodes;
        Token currentToken = Token::Null();
        Token lastToken = Token::Null();
        Token peekedToken = Token::Null();
        unsigned int statementStartPos = 0;

        Token peekToken(bool includeEchoComment = false);
        Token nextToken(bool includeEchoComment = false);
        void pushBackLastToken();
        Node* parseAbsOperator();
        void parseEchosBetweenStatements(Statement* stmt);
    };

