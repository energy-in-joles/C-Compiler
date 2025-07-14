#include "ast_parameter_declaration.hpp"

namespace ast
{

    void ParameterDeclaration::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)stream;
        (void)context;
        (void)destReg;
        (void)type;
    }

    void ParameterDeclaration::Print(std::ostream &stream) const
    {
        stream << declaration_specifiers_ << " ";
        declarator_->Print(stream);
    }

    std::string ParameterDeclaration::GetID() const
    {
        return declarator_->GetID();
    }

    TypeSpecifier ParameterDeclaration::GetType(Context &context) const
    {
        (void)context;
        return declaration_specifiers_;
    }

    bool ParameterDeclaration::IsPointer(Context &context, const bool has_been_declared) const
    {
        (void)has_been_declared;
        return declarator_->IsPointer(context, false);
    }

} // namespace ast
