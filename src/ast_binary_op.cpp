#include "ast_binary_op.hpp"
#include "unordered_map"

namespace ast
{
    const std::unordered_map<char, std::string> operatorStringsBase = {
        {'+', "add"}, // Arithmetic Add
        {'-', "sub"}, // Arithmetic Sub
        {'*', "mul"}, // Arithmetic Mul
        {'/', "div"}, // Arithmetic Div
        {'%', "rem"}, // Arithmetic Rem
        {'&', "and"}, // Bitwise And
        {'|', "or"},  // Bitwise Or
        {'^', "xor"}, // Bitwise XOR
        {'l', "sll"}, // Shift Left
        {'r', "sra"}, // Shift Right
    };

    void BinaryOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        const int srcReg = context.AssignRegister(type);
        expression1_->EmitRISC(stream, context, destReg, type);
        expression2_->EmitRISC(stream, context, srcReg, type);

        // If we are doing pointer arithmetic, we need to multiply the second operand by 4
        // NB: we let all ops work, but technically only add or sub is allowed
        int tmpIndexReg = context.AssignRegister(TypeSpecifier::INT);
        if (expression1_->IsPointer(context, true) && !expression2_->IsPointer(context, true))
        {
            stream << "li " << context.GetRegString(tmpIndexReg) << "," << context.GetPointerOffset(expression1_->GetID(), expression1_->GetPointerDepth()) << std::endl;
            stream << "mul " << context.GetRegString(srcReg) << "," << context.GetRegString(srcReg) << "," << context.GetRegString(tmpIndexReg) << std::endl;
        }
        else if (!expression1_->IsPointer(context, true) && expression2_->IsPointer(context, true))
        {
            stream << "li " << context.GetRegString(tmpIndexReg) << "," << context.GetPointerOffset(expression2_->GetID(), expression1_->GetPointerDepth()) << std::endl;
            stream << "mul " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(tmpIndexReg) << std::endl;
        }
        context.FreeRegister(tmpIndexReg);

        if (type == TypeSpecifier::UNSIGNED)
        {
            if (op_symbol_ == 'r')
            {
                stream << "srl " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(srcReg) << std::endl;
            }
            else if (op_symbol_ == '/')
            {
                stream << "divu " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg) << std::endl;
            }
            else if (op_symbol_ == '%')
            {
                stream << "remu " << context.GetRegString(destReg) << "," << context.GetRegString(srcReg) << std::endl;
            }
            else
            {
                stream << GetArithmeticOp(type, operatorStringsBase.at(op_symbol_)) << " " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(srcReg) << std::endl;
            }
        }
        else
        {
            stream << GetArithmeticOp(type, operatorStringsBase.at(op_symbol_)) << " " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(srcReg) << std::endl;
        }

        context.FreeRegister(srcReg);
    }

    void BinaryOp::Print(std::ostream &stream) const
    {
        expression1_->Print(stream);
        stream << " " << op_symbol_ << " ";
        expression2_->Print(stream);
    }

    std::any BinaryOp::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            return HandleIntOperation(type);
        }
        else if (type == TypeSpecifier::FLOAT)
        {
            return HandleFloatOperation<float>(type);
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            return HandleFloatOperation<double>(type);
        }
        else
        {
            throw std::runtime_error("BinaryOp::GetValue called with invalid type!");
        }
    }

    TypeSpecifier BinaryOp::GetType(Context &context) const
    {
        const TypeSpecifier type1 = expression1_->GetType(context);
        const TypeSpecifier type2 = expression2_->GetType(context);
        if (type1 != type2)
        {
            throw std::runtime_error("BinaryOp::GetType called with mismatched types!");
        }
        return type1;
    }

    std::any BinaryOp::HandleIntOperation(TypeSpecifier type) const
    {
        switch (op_symbol_)
        {
        case '+':
            return EvalOperation<int>(type, std::plus<int>());
        case '-':
            return EvalOperation<int>(type, std::minus<int>());
        case '*':
            return EvalOperation<int>(type, std::multiplies<int>());
        case '/':
            return EvalOperation<int>(type, std::divides<int>());
        case '%':
            return EvalOperation<int>(type, std::modulus<int>());
        case '&':
            return EvalOperation<int>(type, std::bit_and<int>());
        case '|':
            return EvalOperation<int>(type, std::bit_or<int>());
        case '^':
            return EvalOperation<int>(type, std::bit_xor<int>());
        case 'l':
            return std::any((std::any_cast<int>(expression1_->GetValue(type)) << std::any_cast<int>(expression2_->GetValue(type))));
            ;
        case 'r':
            return std::any((std::any_cast<int>(expression1_->GetValue(type)) >> std::any_cast<int>(expression2_->GetValue(type))));
        default:
            throw std::runtime_error("BinaryOp::HandleIntOperation called with invalid operator!");
        }
    }

    template <typename T>
    std::any BinaryOp::HandleFloatOperation(TypeSpecifier type) const
    {
        switch (op_symbol_)
        {
        case '+':
            return EvalOperation<T>(type, std::plus<T>());
        case '-':
            return EvalOperation<T>(type, std::minus<T>());
        case '*':
            return EvalOperation<T>(type, std::multiplies<T>());
        case '/':
            return EvalOperation<T>(type, std::divides<T>());
        default:
            throw std::runtime_error("BinaryOp::HandleFloatOperation called with invalid operator!");
        }
    }

    template <typename T, typename Op>
    std::any BinaryOp::EvalOperation(TypeSpecifier type, Op op) const
    {
        return std::any(op(std::any_cast<T>(expression1_->GetValue(type)),
                           std::any_cast<T>(expression2_->GetValue(type))));
    }

} // namespace ast
