#include "ast_unary_op.hpp"
#include <functional>

namespace ast
{
    void UnaryOp::Print(std::ostream &stream) const
    {
        stream << op_symbol_;
        expression_->Print(stream);
    }

    TypeSpecifier UnaryOp::GetType(Context &context) const
    {
        return expression_->GetType(context);
    }

    void UnaryMinusOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        expression_->EmitRISC(stream, context, destReg, type);
        stream << GetArithmeticOp(type, "neg") << " " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << std::endl;
    }

    void UnaryPlusOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        expression_->EmitRISC(stream, context, destReg, type);
    }

    void UnaryLogicalNotOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string destRegStr = context.GetRegString(destReg);

        expression_->EmitRISC(stream, context, destReg, type);
        stream << "seqz " << destRegStr << "," << destRegStr << std::endl;
        stream << "andi " << destRegStr << "," << destRegStr << ",0xff" << std::endl;
    }

    void UnaryBitwiseNotOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        expression_->EmitRISC(stream, context, destReg, type);
        stream << "not " << context.GetRegString(destReg) << ", " << context.GetRegString(destReg) << std::endl;
    }

    std::any UnaryOp::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::FLOAT)
            return HandleFloatOperation<float>(type);
        else if (type == TypeSpecifier::DOUBLE)
            return HandleFloatOperation<double>(type);
        else if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
            return HandleIntOperation(type);
        else
            throw std::runtime_error("UnaryOp::GetValue called with invalid type!");
    }

    std::any UnaryOp::HandleIntOperation(TypeSpecifier type) const
    {
        switch (op_symbol_)
        {
        case '-':
            return EvalOperation<int>(type, std::negate<int>());
        case '+':
            return std::any_cast<int>(expression_->GetValue(type));
        case '!':
            return !std::any_cast<int>(expression_->GetValue(type)) ? std::any(1) : std::any(0);
        case '~':
            return EvalOperation<int>(type, std::bit_not<int>());
        default:
            throw std::runtime_error("UnaryOp Int Op: Invalid operation");
        }
    }

    template <typename T>
    std::any UnaryOp::HandleFloatOperation(TypeSpecifier type) const
    {
        switch (op_symbol_)
        {
        case '-':
            return EvalOperation<T>(type, std::negate<float>());
        case '+':
            return std::any_cast<T>(expression_->GetValue(type));
        case '!':
            return !std::any_cast<T>(expression_->GetValue(type)) ? std::any(1) : std::any(0);
        default:
            throw std::runtime_error("UnaryOp Float Op: Invalid operation");
        }
    }

    template <typename T, typename Op>
    std::any UnaryOp::EvalOperation(TypeSpecifier type, Op op) const
    {
        return std::any(op(std::any_cast<T>(expression_->GetValue(type))));
    }

} // namespace ast
