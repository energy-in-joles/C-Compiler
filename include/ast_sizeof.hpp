#pragma once
#include "ast_node.hpp"

namespace ast
{
    class SizeOfVar : public Node
    {
    private:
        NodePtr expression_; // nullptr if sizeof(type)

    public:
        SizeOfVar(NodePtr expression)
            : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;

        TypeSpecifier GetType(Context &context) const override;
    };

    class SizeOfType : public Node
    {
    private:
        TypeSpecifier type_; // nullptr if sizeof(type)

    public:
        SizeOfType(TypeSpecifier type)
            : type_(type) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;

        TypeSpecifier GetType(Context &context) const override;
    };
}
