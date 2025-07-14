#pragma once

#include "ast_node.hpp"

namespace ast
{
    class LogicalOp : public Node
    {
    protected:
        NodePtr expression1_;
        NodePtr expression2_;
        std::string op_symbol_ = "LOGICAL_OP";

        template <typename T>
        std::any HandleOperations(TypeSpecifier type) const;
        template <typename T, typename Op>
        std::any EvalOperation(TypeSpecifier type, Op op) const;

    public:
        LogicalOp(NodePtr expression1,
                  NodePtr expression2)
            : expression1_(std::move(expression1)),
              expression2_(std::move(expression2)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override = 0;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

    class LogicalAnd : public LogicalOp
    {
    public:
        LogicalAnd(NodePtr expression1,
                   NodePtr expression2)
            : LogicalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "&&"; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };

    class LogicalOr : public LogicalOp
    {
    public:
        LogicalOr(NodePtr expression1,
                  NodePtr expression2)
            : LogicalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "||"; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
    };
} // namespace ast
