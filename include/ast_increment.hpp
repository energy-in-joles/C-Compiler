#pragma once

#include "ast_node.hpp"

namespace ast
{
    class IncAndDecOp : public Node
    {
    protected:
        char op_symbol_;
        std::string addi_value_;
        NodePtr expression_;
        void SetAddiValue(char op_symbol_);
        std::string GetAddOrSubOp(TypeSpecifier type) const;

    public:
        IncAndDecOp(char op_symbol, NodePtr expression)
            : op_symbol_(op_symbol), expression_(std::move(expression)) { SetAddiValue(op_symbol_); }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override = 0;
        void Print(std::ostream &stream) const override = 0;
        TypeSpecifier GetType(Context &context) const override;
    };

    class PostIncAndDecOp : public IncAndDecOp
    {
    public:
        PostIncAndDecOp(char op_symbol, NodePtr expression)
            : IncAndDecOp(op_symbol, std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

    class PreIncAndDecOp : public IncAndDecOp
    {
    protected:
        char op_symbol_;

    public:
        PreIncAndDecOp(char op_symbol, NodePtr expression)
            : IncAndDecOp(op_symbol, std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
