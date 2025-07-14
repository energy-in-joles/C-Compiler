#pragma once

#include "ast_node.hpp"
#include <string>

namespace ast
{

    class ReturnStatement : public Node
    {
    private:
        NodePtr expression_;
        std::string functionLabel;

    public:
        ReturnStatement(NodePtr expression) : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
