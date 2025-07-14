#pragma once

#include "ast_node.hpp"
#include "ast_type_specifier.hpp"

namespace ast
{

    class Declaration : public Node
    {
    private:
        const TypeSpecifier declaration_specifiers_;
        NodePtr declarator_list_;

    public:
        Declaration(TypeSpecifier type, NodePtr declarator_list)
            : declaration_specifiers_(type),
              declarator_list_(std::move(declarator_list)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
