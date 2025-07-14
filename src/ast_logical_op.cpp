#include "ast_logical_op.hpp"

namespace ast
{
    void LogicalOp::Print(std::ostream &stream) const
    {
        stream << "(";
        expression1_->Print(stream);
        stream << " " << op_symbol_ << " ";
        expression2_->Print(stream);
        stream << ")";
    }

    void LogicalAnd::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string label_short = context.GenerateUniqueLabel("and_short");
        std::string label_end = context.GenerateUniqueLabel("and_end");
        std::string destRegStr = context.GetRegString(destReg);

        if (type != TypeSpecifier::INT)
        {
            throw std::runtime_error("LogicalAnd::EmitRISC: invalid type");
        }
        // evaluate expressions
        expression1_->EmitRISC(stream, context, destReg, type);
        stream << "beq " << destRegStr << ",zero," << label_short << std::endl;
        expression2_->EmitRISC(stream, context, destReg, type);
        stream << "beq " << destRegStr << ",zero," << label_short << std::endl;
        stream << "li " << destRegStr << ",1" << std::endl;
        stream << "j " << label_end << std::endl;

        // short circuit
        stream << label_short << ":" << std::endl;
        stream << "li " << destRegStr << ",0" << std::endl;

        // end
        stream << label_end << ":" << std::endl;
    }

    void LogicalOr::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string label_short_true = context.GenerateUniqueLabel("or_short_true");
        std::string label_false = context.GenerateUniqueLabel("or_false");
        std::string label_end = context.GenerateUniqueLabel("or_end");
        std::string destRegStr = context.GetRegString(destReg);

        if (type != TypeSpecifier::INT)
        {
            throw std::runtime_error("LogicalOr::EmitRISC: invalid type");
        }
        // evaluate expressions
        expression1_->EmitRISC(stream, context, destReg, type);
        stream << "bne " << destRegStr << ",zero," << label_short_true << std::endl;
        expression2_->EmitRISC(stream, context, destReg, type);
        stream << "beq " << destRegStr << ",zero," << label_false << std::endl;

        // short circuit
        stream << label_short_true << ":" << std::endl;
        stream << "li " << destRegStr << ",1" << std::endl;
        stream << "j " << label_end << std::endl;

        // false path
        stream << label_false << ":" << std::endl;
        stream << "li " << destRegStr << ",0" << std::endl;

        // end
        stream << label_end << ":" << std::endl;
    }

    std::any LogicalOp::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            return HandleOperations<int>(type);
        }
        else if (type == TypeSpecifier::FLOAT)
        {
            return HandleOperations<float>(type);
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            return HandleOperations<double>(type);
        }
        else
        {
            throw std::runtime_error("LogicalOp::GetValue called with invalid type!");
        }
    }

    TypeSpecifier LogicalOp::GetType(Context &context) const
    {
        (void)context;
        return TypeSpecifier::INT;
    }

    template <typename T>
    std::any LogicalOp::HandleOperations(TypeSpecifier type) const
    {
        if (op_symbol_ == "&&")
        {
            return EvalOperation<T>(type, std::logical_and<int>());
        }
        else if (op_symbol_ == "||")
        {
            return EvalOperation<T>(type, std::logical_or<int>());
        }
        else
        {
            throw std::runtime_error("LogicalOp::HandleOperations: invalid operator");
        }
    }

    template <typename T, typename Op>
    std::any LogicalOp::EvalOperation(TypeSpecifier type, Op op) const
    {
        return op(std::any_cast<T>(expression1_->GetValue(type)),
                  std::any_cast<T>(expression2_->GetValue(type)))
                   ? std::any(1)
                   : std::any(0);
    }

} // namespace ast
