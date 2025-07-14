#pragma once

#include "ast_node.hpp"
#include "functional"

namespace ast
{
    class BinaryOp : public Node
    {
    private:
        std::any HandleIntOperation(TypeSpecifier type) const;
        template <typename T>
        std::any HandleFloatOperation(TypeSpecifier type) const;
        template <typename T, typename Op>
        std::any EvalOperation(TypeSpecifier type, Op op) const;

        std::string GetOperatorString(TypeSpecifier type) const;

    protected:
        const char op_symbol_;
        NodePtr expression1_;
        NodePtr expression2_;

    public:
        BinaryOp(char op_symbol,
                 NodePtr expression1,
                 NodePtr expression2)
            : op_symbol_(op_symbol),
              expression1_(std::move(expression1)),
              expression2_(std::move(expression2)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

} // namespace ast
