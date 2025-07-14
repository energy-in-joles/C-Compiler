#pragma once

#include "ast_node.hpp"

namespace ast
{

    class Identifier : public Node
    {
    private:
        std::string identifier_;

    public:
        Identifier(std::string identifier) : identifier_(std::move(identifier)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;
        bool IsFunction() const override { return false; };
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
    };

} // namespace ast
