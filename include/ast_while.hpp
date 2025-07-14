#pragma once

#include "ast_node.hpp"

namespace ast
{

    class WhileLoop : public Node
    {
    private:
        NodePtr condition_;
        NodePtr body_;

    public:
        WhileLoop(NodePtr condition, NodePtr body) : condition_(std::move(condition)), body_(std::move(body)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };
}
