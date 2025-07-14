#include "ast_array_declarator.hpp"
#include "ast_constant.hpp"
#include "ast_identifier.hpp"
#include "ast_direct_declarator.hpp"
#include <typeinfo>
#include <any>
#include <string>
#include <iostream>

namespace ast
{

    void ArrayDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)stream;
        (void)context;
        (void)destReg;
        (void)type;
        throw std::runtime_error("ArrayDeclarator: EmitRISC not implemented.");
    }

    std::string ArrayDeclarator::GetID() const
    {
        return identifier_->GetID();
    }

    bool ArrayDeclarator::IsFunction() const
    {
        return false;
    }

    bool ArrayDeclarator::IsArray() const
    {
        return true;
    }

    int ArrayDeclarator::GetArraySize(Context &context) const
    {
        (void)context;
        if (size_ == nullptr)
        {
            return -1;
        }
        return std::any_cast<int>(size_->GetValue(TypeSpecifier::INT));
    }

    void ArrayDeclarator::Print(std::ostream &stream) const
    {
        identifier_->Print(stream);
        stream << "[";
        if (size_ != nullptr)
            size_->Print(stream);
        stream << "]";
    }

} // namespace ast
