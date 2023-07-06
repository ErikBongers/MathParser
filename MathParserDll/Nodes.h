#pragma once
#include "Error.h"
#include <vector>
#include "ValueType.h"
#include "Range.h"
#include "Unit.h"
#include "Token.h"

enum class NodeType {FUNCTIONDEF, CONSTEXPR, POSTFIXEXPR, IDEXPR, CALLEXPR, BINARYOPEXPR, UNARYOPEXPR, ASSIGNMENT, STATEMENT, DEFINE, LIST, NONE};
class NodeFactory;
struct CodeBlock;

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
        Range range() const { return token.range; }
    private:
        ~NoneExpr() {}
        NoneExpr() : Node(NodeType::NONE) {};
        friend class NodeFactory;
    };

enum class ConstType : char { NUMBER = 'N', FORMATTED_STRING = 'S' };

class ConstExpr : public Node
    {
    public:
        ConstType type;
        Token value;
        Range range() const;
    private:
        ~ConstExpr() {}
        ConstExpr(ConstType type) : Node(NodeType::CONSTEXPR), type(type) {};
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
        std::string functionName; //this may not be a stream range, but a translated function name: e.g. x++ -> _inc(x)
        Range functionNameRange; // original range in the stream
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
        CodeBlock* codeBlock = nullptr;
        Node* node = nullptr;
        Range text;
        Range comment_line;
        bool mute = false; //mute output
        bool echo = false; //echo statement
        Range range() const;
    private:
        ~Statement();
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
        std::string id; //id may be a decorated name in case of polymorphism.
        Range idRange; //original range in stream.
        std::vector<std::string> params;
        Range range() const;
    };
