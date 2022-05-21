#pragma once
#include "Error.h"
#include "Tokenizer.h"
#include <vector>
#include "Function.h"
#include "ValueType.h"
#include "Range.h"

enum class NodeType {FUNCTIONDEF, CONSTEXPR, POSTFIXEXPR, IDEXPR, CALLEXPR, BINARYOPEXPR, UNARYOPEXPR, ASSIGNMENT, STATEMENT, DEFINE, LIST, NONE};
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
        virtual Range range() const = 0;
        virtual ~Node() {}
    private:
        Node() : type(NodeType::CONSTEXPR) {}
        friend class Parser;
    };

class NoneExpr : public Node
    {
    public:
        Token token;
        Range range() const { return Range(token); }
    private:
        NoneExpr() : Node(NodeType::NONE) {};
        friend class Parser;
    };

class ConstExpr : public Node
    {
    public:
        ValueType type;
        Token value;
        Range range() const;
    private:
        ConstExpr(ValueType type) : Node(NodeType::CONSTEXPR), type(type) {};
        friend class Parser;
    };

class IdExpr : public Node
    {
    public:
        Token Id;
        Range range() const;
    private:
        IdExpr() : Node(NodeType::IDEXPR) {}
        friend class Parser;
    };

class PostfixExpr : public Node
    {
    public:
        Node* expr = nullptr;
        Token postfixId;
        Range range() const;
    private:
        PostfixExpr() : Node(NodeType::POSTFIXEXPR) {}
        friend class Parser;
    };


class CallExpr : public Node
    {
    public:
        Token functionName;
        std::vector<Node*> arguments;
        Range range() const;
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
        Range range() const;
    private:
        BinaryOpExpr() : Node(NodeType::BINARYOPEXPR) {}
        friend class Parser;
    };

class UnaryOpExpr : public Node
    {
    public:
        Token op;
        Node* n = nullptr;
        Range range() const;
    private:
        UnaryOpExpr() : Node(NodeType::UNARYOPEXPR) {}
        friend class Parser;
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* expr = nullptr;
        Range range() const;
    private:
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class Parser;
    };

class ListExpr : public Node
    {
    public: 
        std::vector<Node*> list;
        Range range() const;
    private:
        ListExpr() : Node(NodeType::LIST) {}
        friend class Parser;
    };

class Define : public Node
    {
    public:
        std::vector<Token> defs;
        Range range() const;
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
        Range range() const;
    private:
        Statement() : Node(NodeType::STATEMENT) {}
        friend class Parser;
    };

class FunctionDefExpr : public Node
    {
    private:
        FunctionDefExpr() : Node(NodeType::FUNCTIONDEF) {}
        friend class Parser;
    public:
        Token id;
        std::vector<Token> params;
        std::vector<Statement*> statements;
        Range range() const;
    };
