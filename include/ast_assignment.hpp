#pragma once
#include "ast_node.hpp"

namespace ast
{

    class Assignment : public Node
    {
    private:
        NodePtr destination_;
        const std::string assignment_str_;
        NodePtr source_;
        void EmitPointerDereference(std::ostream &stream, Context &context, int destReg) const;

    public:
        Assignment(NodePtr destination, const std::string assignment_str, NodePtr source)
            : destination_(std::move(destination)), assignment_str_(assignment_str), source_(std::move(source)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
