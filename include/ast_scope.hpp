#pragma once

#include "ast_node.hpp"
#include <vector>

namespace ast
{

    class Scope : public Node
    {
    private:
        NodeList statements_;

    public:
        Scope(NodeList statements) : statements_(std::move(statements)) {}

        const NodeList &GetNodes() const { return statements_; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
