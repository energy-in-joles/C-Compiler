#include "ast_relational_op.hpp"
#include <functional>

namespace ast
{
    std::string GetRelationalOpString(std::string base_op_str, TypeSpecifier type)
    {
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            return "s" + base_op_str;
        }
        else if (type == TypeSpecifier::FLOAT)
        {
            return "f" + base_op_str + ".s";
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            return "f" + base_op_str + ".d";
        }
        else
        {
            throw std::runtime_error("GetRelationalOpString called with invalid type");
        }
    }

    void RelationalOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        if (type != TypeSpecifier::INT)
        {
            throw std::runtime_error("RelationalOp::EmitRISC: expected return type must be INT");
        }

        TypeSpecifier type1 = expression1_->GetType(context);
        TypeSpecifier type2 = expression2_->GetType(context);

        type1 = (expression1_->IsPointer(context, true)) ? TypeSpecifier::INT : type1;
        type2 = (expression2_->IsPointer(context, true)) ? TypeSpecifier::INT : type2;

        int srcReg1 = context.AssignRegister(type1);
        int srcReg2 = context.AssignRegister(type2);
        // evaluate both expressions
        EmitStart(stream, context, srcReg1, srcReg2, type1);
        EmitMain(stream, context, destReg, srcReg1, srcReg2, type1);
        EmitEnd(stream, context, destReg);

        context.FreeRegister(srcReg1);
        context.FreeRegister(srcReg2);
    }

    void RelationalOp::EmitStart(std::ostream &stream, Context &context, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        expression1_->EmitRISC(stream, context, srcReg1, type);
        expression2_->EmitRISC(stream, context, srcReg2, type);
    }

    void RelationalOp::EmitEnd(std::ostream &stream, Context &context, int destReg) const
    {
        stream << "andi " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",0xff" << std::endl;
    }

    void RelationalOp::Print(std::ostream &stream) const
    {
        expression1_->Print(stream);
        stream << " " << op_symbol_ << " ";
        expression2_->Print(stream);
    }

    TypeSpecifier RelationalOp::GetType(Context &context) const
    {
        (void)context;
        // relational is always an int
        return TypeSpecifier::INT;
    }

    // NB: make sure that result is stored in destReg for ALL EmitMain
    void LessThan::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        stream << GetRelationalOpString("lt", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
    }

    void LessThanEqual::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        stream << GetRelationalOpString("gt", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
        stream << "xori " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",1" << std::endl;
    }

    void GreaterThan::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        stream << GetRelationalOpString("gt", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
    }

    void GreaterThanEqual::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        // signed for integer
        stream << GetRelationalOpString("lt", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
        stream << "xori " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",1" << std::endl;
    }

    void Equal::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        // TODO: this needs to work for chars too
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            stream << "sub " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
            stream << "seqz " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << std::endl;
        }
        else
        {
            stream << GetRelationalOpString("eq", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
        }
    }

    void NotEqual::EmitMain(std::ostream &stream, Context &context, int destReg, int srcReg1, int srcReg2, TypeSpecifier type) const
    {
        // TODO: this needs to work for chars too
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            stream << "sub " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
            stream << "snez " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << std::endl;
        }
        else
        {
            stream << GetRelationalOpString("eq", type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg1) << "," << context.GetRegString(srcReg2) << std::endl;
            stream << "xori " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",1" << std::endl;
        }
    }

    std::any RelationalOp::GetValue(TypeSpecifier type) const
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
            throw std::runtime_error("RelationalOp::GetValue called with invalid type!");
        }
    }

    template <typename T>
    std::any RelationalOp::HandleOperations(TypeSpecifier type) const
    {
        if (op_symbol_ == "<")
        {
            return EvalOperation<T>(type, std::less<T>());
        }
        else if (op_symbol_ == "<=")
        {
            return EvalOperation<T>(type, std::less_equal<T>());
        }
        else if (op_symbol_ == ">")
        {
            return EvalOperation<T>(type, std::greater<T>());
        }
        else if (op_symbol_ == ">=")
        {
            return EvalOperation<T>(type, std::greater_equal<T>());
        }
        else if (op_symbol_ == "==")
        {
            return EvalOperation<T>(type, std::equal_to<T>());
        }
        else if (op_symbol_ == "!=")
        {
            return EvalOperation<T>(type, std::not_equal_to<T>());
        }
        else
        {
            throw std::runtime_error("RelationalOp::HandleOperations called with invalid operator!");
        }
    }

    template <typename T, typename Op>
    std::any RelationalOp::EvalOperation(TypeSpecifier type, Op op) const
    {
        return op(std::any_cast<T>(expression1_->GetValue(type)),
                  std::any_cast<T>(expression2_->GetValue(type)))
                   ? std::any(1)
                   : std::any(0);
    }

} // namespace ast
