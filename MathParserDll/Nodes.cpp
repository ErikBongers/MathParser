#include "pch.hpp"
#include "Nodes.h"

Range ConstExpr::range() const
    {
    return value.range;
    }

Range IdExpr::range() const
    {
    return Id.range;
    }

Range PostfixExpr::range() const
    {
    Range r = postfixId.range;
    if(expr != nullptr)
        r += expr->range();
    return r;
    }

Range CallExpr::range() const
    {
    Range r = functionNameRange;
    if(arguments != nullptr && !arguments->list.empty())
        r += arguments->list.back()->range();
    return r;
    }

Range BinaryOpExpr::range() const
    {
    Range r = Range(n1->range());
    r += Range(n2->range());
    return r;
    }

Range UnaryOpExpr::range() const
    {
    Range r = Range(n->range());
    r += op.range;
    return r;
    }

Range AssignExpr::range() const
    {
    Range r = Id.range;
    if(expr != nullptr)
        r += expr->range();
    return r;
    }

Range ListExpr::range() const
    {
    if(!list.empty())
        {
        Range r = Range(list.front()->range());
        r += list.back()->range();
        return r;
        }
    return Range();
    }

Range Define::range() const
    {
    Range r = def_undef.range;

    if(defs.size() == 0)
        return r;

    r += defs.front().range;
    r += defs.back().range;
    return r;
    }

Range Statement::range() const
    {
    if(node != nullptr)
        return node->range();
    return comment_line;
    }

Range FunctionDefExpr::range() const
    {
    return r;
    }
