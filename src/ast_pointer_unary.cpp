#include "ast_pointer_unary.hpp"

namespace ast
{
    void UnaryAddressOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        if (context.VariableIsLocal(expression_->GetID()))
        {
            stream << "addi " << context.GetRegString(destReg) << ",s0," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(expression_->GetID()).offset << std::endl;
        }
        else
        {
            stream << "lui " << context.GetRegString(destReg) << ",%hi(" << expression_->GetID() << ")" << std::endl;
            stream << "addi " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",%lo(" << expression_->GetID() << ")" << std::endl;
        }
    }

    void UnaryAddressOp::Print(std::ostream &stream) const
    {
        stream << "&";
        expression_->Print(stream);
    }

    TypeSpecifier UnaryAddressOp::GetType(Context &context) const
    {
        (void)context;
        return TypeSpecifier::INT;
    }

    bool UnaryAddressOp::IsPointer(Context &context, const bool has_been_declared) const
    {
        (void)context;
        (void)has_been_declared;
        return true;
    }

    void UnaryDereferenceOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        int srcReg;
        if (!IsPointer(context, true))
        {
            srcReg = context.AssignRegister(TypeSpecifier::INT);
        }
        else
        {
            srcReg = destReg;
        }
        expression_->EmitRISC(stream, context, srcReg, type);

        if (IsPointer(context, true))
        {
            stream << "lw " << context.GetRegString(destReg) << ",0(" << context.GetRegString(srcReg) << ")" << std::endl;
        }
        else
        {
            stream << GetLoadOp(GetType(context)) << " " << context.GetRegString(destReg) << ",0(" << context.GetRegString(srcReg) << ")" << std::endl;
        }

        if (destReg != srcReg)
        {
            context.FreeRegister(srcReg);
        }
    }

    void UnaryDereferenceOp::Print(std::ostream &stream) const
    {
        stream << "*";
        expression_->Print(stream);
    }

    void UnaryDereferenceOp::UpdatePointerDepth()
    {
        pointer_depth_ = expression_->GetPointerDepth() + 1;
    }

    bool UnaryDereferenceOp::IsPointer(Context &context, const bool has_been_declared) const
    {
        (void)has_been_declared;
        int ptr_depth;
        if (context.VariableIsLocal(expression_->GetID()))
        {
            ptr_depth = context.GetCurrentFunctionContext().GetFunctionVariableInfo(expression_->GetID()).pointer_depth;
        }
        else
        {
            ptr_depth = context.GetGlobalVariableInfo(expression_->GetID()).pointer_depth;
        }

        return (ptr_depth > pointer_depth_) ? true : false;
    }

    int UnaryDereferenceOp::GetPointerDepth() const
    {
        return pointer_depth_;
    }

    std::string PointerUnary::GetID() const
    {
        return expression_->GetID();
    }

    TypeSpecifier UnaryDereferenceOp::GetType(Context &context) const
    {
        return expression_->GetType(context);
    }
} // namespace ast
