#pragma once

#include "ast_node.hpp"

namespace ast
{

    class ForStatement : public Node
    {
    private:
        NodePtr init_assignment_;
        NodePtr condition_;
        NodePtr update_assignment_;
        NodePtr for_body_;

    public:
        ForStatement(NodePtr init_assignment, NodePtr condition, NodePtr update_assignment, NodePtr body)
            : init_assignment_(std::move(init_assignment)),
              condition_(std::move(condition)),
              update_assignment_(std::move(update_assignment)),
              for_body_(std::move(body)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };
}
