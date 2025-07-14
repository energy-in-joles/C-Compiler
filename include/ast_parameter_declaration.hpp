#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast
{

    class ParameterDeclaration : public Node
    {
    private:
        const TypeSpecifier declaration_specifiers_;
        NodePtr declarator_;

    public:
        ParameterDeclaration(TypeSpecifier declaration_specifiers, NodePtr declarator) : declaration_specifiers_(declaration_specifiers), declarator_(std::move(declarator)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
    };

} // namespace ast
