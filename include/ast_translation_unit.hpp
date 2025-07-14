#pragma once
#include "ast_node.hpp"

namespace ast
{

    class TranslationUnit : public Node
    {
    private:
        NodePtr external_declarations_;

    public:
        TranslationUnit(NodePtr external_declarations)
            : external_declarations_(std::move(external_declarations)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
