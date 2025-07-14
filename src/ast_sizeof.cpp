#include "ast_sizeof.hpp"
#include "ast_context.hpp"
#include "ast_type_specifier.hpp"
#include <iostream>

namespace ast
{
    void SizeOfVar::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type; // Unused

        // sizeof(expression)
        TypeSpecifier expr_type = expression_->GetType(context);
        int size = GetTypeSize(expr_type);

        // Load the size into the destination register
        stream << "li " << context.GetRegString(destReg) << ", " << size << " # sizeof(expression)" << std::endl;
    }

    void SizeOfVar::Print(std::ostream &stream) const
    {
        stream << "sizeof(";
        expression_->Print(stream);
        stream << ")";
    }

    TypeSpecifier SizeOfVar::GetType(Context &context) const
    {
        (void)context; // Unused
        return TypeSpecifier::INT;
    }

    void SizeOfType::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type; // Unused

        // sizeof(type)
        int size = GetTypeSize(type_);

        // Load the size into the destination register
        stream << "li " << context.GetRegString(destReg) << ", " << size << " # sizeof(type)" << std::endl;
    }

    void SizeOfType::Print(std::ostream &stream) const
    {
        stream << "sizeof(" << type_ << ")";
    }

    TypeSpecifier SizeOfType::GetType(Context &context) const
    {
        (void)context; // Unused
        return TypeSpecifier::INT;
    }
}
