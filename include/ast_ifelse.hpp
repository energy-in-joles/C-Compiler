#pragma once

#include "ast_node.hpp"

namespace ast
{

    class IfStatement : public Node
    {
    private:
        NodePtr condition_;
        NodePtr ifBody_;
        NodePtr elseBody_;

    public:
        IfStatement(NodePtr condition, NodePtr ifBody, NodePtr elseBody) : condition_(std::move(condition)), ifBody_(std::move(ifBody)), elseBody_(std::move(elseBody)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;

        TypeSpecifier GetType(Context &context) const override;
    };
}
