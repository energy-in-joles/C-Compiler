#include "ast_pointer_declarator.hpp"
#include "ast_direct_declarator.hpp"

namespace ast
{
    // this is used as a pssthrough for pointer return type functions
    void PointerDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        declarator_->EmitRISC(stream, context, destReg, type);
    }

    std::string PointerDeclarator::GetID() const
    {
        // return (std::string(pointer_depth_, '*') + declarator_->GetID());
        return declarator_->GetID();
    }

    void PointerDeclarator::Print(std::ostream &stream) const
    {
        stream << std::string(pointer_depth_, '*');
        declarator_->Print(stream);
    }

    bool PointerDeclarator::IsPointer(Context &context, const bool has_been_declared) const
    {
        (void)context;
        (void)has_been_declared;
        return true;
    }

    int PointerDeclarator::GetPointerDepth() const
    {
        return pointer_depth_;
    }

    bool PointerDeclarator::IsArray() const
    {
        return declarator_->IsArray();
    }

    int PointerDeclarator::GetArraySize(Context &context) const
    {
        return declarator_->GetArraySize(context);
    }

    bool PointerDeclarator::IsFunction() const
    {
        return declarator_->IsFunction();
    }

    void PointerDeclarator::StoreFunctionInfo(TypeSpecifier return_type, Context &context) const
    {
        if (!declarator_->IsFunction())
        {
            throw std::runtime_error("PointerDeclarator::StoreFunctionInfo called on non-function");
        }
        const DirectDeclarator &func_declarator = dynamic_cast<const DirectDeclarator &>(*declarator_);
        func_declarator.StoreFunctionInfo(return_type, context);
    }
} // namespace ast
