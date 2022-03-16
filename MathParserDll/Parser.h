#pragma once
#include "Unit.h"
#include "Error.h"
#include "Tokenizer.h"
#include <vector>
#include "Function.h"

enum class NodeType {CONSTEXPR, POSTFIXEXPR, PRIMARYEXPR, CALLEXPR, BINARYOPEXPR, ASSIGNMENT, STATEMENT};
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
        Token constNumber;
    private:
        ConstExpr() : Node(NodeType::CONSTEXPR) {};
        friend class Parser;
    };

class PrimaryExpr : public Node
    {
    public:
        Token Id;
        Node* addExpr = nullptr; //TODO: get rid of this?
        Node* callExpr = nullptr; //TODO: get rid of this
    private:
        PrimaryExpr() : Node(NodeType::PRIMARYEXPR) {}
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

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* expr = nullptr;
    private:
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class Parser;
    };

class Statement : public Node
    {
    public:
        Node* assignExpr = nullptr;
        Node* addExpr = nullptr;
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
    public:
        FunctionDefs& functionDefs;
        std::map<std::string, Variable> ids;
        std::vector<Statement*> statements;
        bool muteBlock = false;
        bool echoBlock = false;
        bool echoTrailingComment = false;

        Parser(const char* stream, FunctionDefs& functionDefs);
        void parse();
        Statement* parseStatement();
        Statement* parseStatementHeader(Statement* stmt);
        Statement* parseStatementBody(Statement* stmt);
        Node* parseAssignExpr();
        Node* parseAddExpr();
        Node* parseMultExpr();
        Node* parsePowerExpr();
        Node* parseImplicitMult();
        Node* parseUnitExpr();
        Node* parsePostFixExpr();
        Node* parseOnePostFix(Node* node, Token t);
        Node* parsePrimaryExpr();
        ConstExpr* parseConst(bool negative);
        CallExpr* parseCallExpr(Token functionName);
        ~Parser() { for (auto node : nodes) delete node; }
        ConstExpr* createConst();
        BinaryOpExpr* createBinaryOp();
        PrimaryExpr* createPrimary();
        PostfixExpr* createPostfix();
        AssignExpr* createAssign();
        CallExpr* createCall();
        Statement* createStatement();
    private:
        std::vector<Node*> nodes;
        Token currentToken = Token::Null();
        Token lastToken = Token::Null();
        Token peekedToken = Token::Null();
        unsigned int statementStartPos = 0;

        Token peekToken(bool includeEchoComment = false);
        Token nextToken(bool includeEchoComment = false);
        void pushBackLastToken();
    };

