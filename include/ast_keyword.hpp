#pragma once

#include "ast_node.hpp"

namespace ast
{

    class ContinueStatement : public Node
    {
    public:
        ContinueStatement() {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

    class BreakStatement : public Node
    {
    public:
        BreakStatement() {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };
}
