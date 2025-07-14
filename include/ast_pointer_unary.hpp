#pragma once

#include "ast_node.hpp"

namespace ast
{

    class PointerUnary : public Node
    {
    protected:
        NodePtr expression_;

    public:
        PointerUnary(NodePtr expression) : expression_(std::move(expression)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override = 0;
        void Print(std::ostream &stream) const override = 0;
        TypeSpecifier GetType(Context &context) const override = 0;
        std::string GetID() const override;
    };

    class UnaryAddressOp : public PointerUnary
    {
    public:
        UnaryAddressOp(NodePtr expression) : PointerUnary(std::move(expression)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
    };

    class UnaryDereferenceOp : public PointerUnary
    {
    private:
        int pointer_depth_ = 1;

    public:
        UnaryDereferenceOp(NodePtr expression) : PointerUnary(std::move(expression)) { UpdatePointerDepth(); };

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
        int GetPointerDepth() const override;

        void UpdatePointerDepth();
    };

} // namespace ast
