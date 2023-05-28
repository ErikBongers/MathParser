#pragma once
#include "Error.h"
#include <vector>
#include "ValueType.h"
#include "Range.h"
#include "Unit.h"
#include "Token.h"

enum class NodeType {FUNCTIONDEF, CONSTEXPR, POSTFIXEXPR, IDEXPR, CALLEXPR, BINARYOPEXPR, UNARYOPEXPR, ASSIGNMENT, STATEMENT, DEFINE, LIST, NONE};
class NodeFactory;

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
    protected:
        Node() : type(NodeType::CONSTEXPR) {}
    private:
        friend class NodeFactory;
    };

class NoneExpr : public Node
    {
    public:
        Token token;
        Range range() const { return Range(token); }
    private:
        ~NoneExpr() {}
        NoneExpr() : Node(NodeType::NONE) {};
        friend class NodeFactory;
    };

class ConstExpr : public Node
    {
    public:
        ValueType type;
        Token value;
        Range range() const;
    private:
        ~ConstExpr() {}
        ConstExpr(ValueType type) : Node(NodeType::CONSTEXPR), type(type) {};
        friend class NodeFactory;
    };

class IdExpr : public Node
    {
    public:
        Token Id;
        Range range() const;
    private:
        ~IdExpr() {}
        IdExpr() : Node(NodeType::IDEXPR) {}
        friend class NodeFactory;
    };

class PostfixExpr : public Node
    {
    public:
        Node* expr = nullptr;
        Token postfixId;
        Range range() const;
    private:
        ~PostfixExpr() {}
        PostfixExpr() : Node(NodeType::POSTFIXEXPR) {}
        friend class NodeFactory;
    };

class ListExpr : public Node
    {
    public: 
        std::vector<Node*> list;
        Range range() const;
    private:
        ~ListExpr() {}
        ListExpr() : Node(NodeType::LIST) {}
        friend class NodeFactory;
    };

class CallExpr : public Node
    {
    public:
        Token functionName;
        ListExpr* arguments = nullptr;
        Range range() const;
    private:
        CallExpr() : Node(NodeType::CALLEXPR) {}
        ~CallExpr() {}
        friend class NodeFactory;
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
        ~BinaryOpExpr() {}
        BinaryOpExpr() : Node(NodeType::BINARYOPEXPR) {}
        friend class NodeFactory;
    };

class UnaryOpExpr : public Node
    {
    public:
        Token op;
        Node* n = nullptr;
        Range range() const;
    private:
        ~UnaryOpExpr() {}
        UnaryOpExpr() : Node(NodeType::UNARYOPEXPR) {}
        friend class NodeFactory;
    };

class AssignExpr : public Node
    {
    public: 
        Token Id;
        Node* expr = nullptr;
        Range range() const;
    private:
        ~AssignExpr() {}
        AssignExpr() : Node(NodeType::ASSIGNMENT) {}
        friend class NodeFactory;
    };

class Define : public Node
    {
    public:
        Token def_undef; //needed for range
        std::vector<Token> defs;
        Range range() const;
        bool undef = false;
    private:
        ~Define() {}
        Define() : Node(NodeType::DEFINE) {}
        friend class NodeFactory;
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
        ~Statement() {}
        Statement() : Node(NodeType::STATEMENT) {}
        friend class NodeFactory;
    };

class FunctionDefExpr : public Node
    {
    private:
        FunctionDefExpr() : Node(NodeType::FUNCTIONDEF) {}
        ~FunctionDefExpr() {}
        friend class NodeFactory;
    public:
        Range r;
        Token id;
        std::vector<Token> params;
        Range range() const;
    };
