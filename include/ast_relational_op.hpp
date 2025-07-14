#pragma once

#include "ast_node.hpp"

namespace ast
{
    class RelationalOp : public Node
    {
    protected:
        NodePtr expression1_;
        NodePtr expression2_;
        std::string op_symbol_ = "RELATIONAL_OP";

    public:
        RelationalOp(NodePtr expression1,
                     NodePtr expression2)
            : expression1_(std::move(expression1)),
              expression2_(std::move(expression2)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        // use start and end to standardise the common parts of EmitRISC
        void EmitStart(std::ostream &stream, Context &context, int srcReg1, int srcReg2, TypeSpecifier type) const;
        void EmitEnd(std::ostream &stream, Context &context, int destReg) const;
        virtual void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const = 0;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const override;

        std::any GetValue(TypeSpecifier type) const override;
        template <typename T>
        std::any HandleOperations(TypeSpecifier type) const;
        template <typename T, typename Op>
        std::any EvalOperation(TypeSpecifier type, Op op) const;
    };

    class LessThan : public RelationalOp
    {
    public:
        LessThan(NodePtr expression1,
                 NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "<"; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

    class LessThanEqual : public RelationalOp
    {
    public:
        LessThanEqual(NodePtr expression1,
                      NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "<="; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

    class GreaterThan : public RelationalOp
    {
    public:
        GreaterThan(NodePtr expression1,
                    NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = ">"; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

    class GreaterThanEqual : public RelationalOp
    {
    public:
        GreaterThanEqual(NodePtr expression1,
                         NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = ">="; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

    class Equal : public RelationalOp
    {
    public:
        Equal(NodePtr expression1,
              NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "=="; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

    class NotEqual : public RelationalOp
    {
    public:
        NotEqual(NodePtr expression1,
                 NodePtr expression2)
            : RelationalOp(std::move(expression1), std::move(expression2)) { op_symbol_ = "!="; }

        void EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const override;
    };

} // namespace ast
