#pragma once

#include "ast_node.hpp"

namespace ast
{

    class UnaryOp : public Node
    {
    protected:
        NodePtr expression_;
        char op_symbol_ = 'U';

        std::any HandleIntOperation(TypeSpecifier type) const;
        template <typename T>
        std::any HandleFloatOperation(TypeSpecifier type) const;
        template <typename T, typename Op>
        std::any EvalOperation(TypeSpecifier type, Op op) const;

    public:
        UnaryOp(NodePtr expression) : expression_(std::move(expression)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override = 0;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

    class UnaryMinusOp : public UnaryOp
    {
    public:
        UnaryMinusOp(NodePtr expression) : UnaryOp(std::move(expression)) { op_symbol_ = '-'; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };

    class UnaryPlusOp : public UnaryOp
    {
    public:
        UnaryPlusOp(NodePtr expression) : UnaryOp(std::move(expression)) { op_symbol_ = '+'; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };

    class UnaryLogicalNotOp : public UnaryOp
    {
    public:
        UnaryLogicalNotOp(NodePtr expression) : UnaryOp(std::move(expression)) { op_symbol_ = '!'; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };

    class UnaryBitwiseNotOp : public UnaryOp
    {
    public:
        UnaryBitwiseNotOp(NodePtr expression) : UnaryOp(std::move(expression)) { op_symbol_ = '~'; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };

} // namespace ast
