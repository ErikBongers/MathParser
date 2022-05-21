#pragma once
#include "Token.h"
#include "Nodes.h"

class Variable
    {
    public:
        Token name;
        Node* addExpr = nullptr;
    };
