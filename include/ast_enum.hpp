#pragma once

#include "ast_node.hpp"

namespace ast
{

    class EnumDeclaration : public Node
    {
    private:
        const std::string enum_name_;
        NodePtr enumerators_;

    public:
        EnumDeclaration(const std::string &enum_name, NodePtr enumerators)
            : enum_name_(enum_name), enumerators_(std::move(enumerators)) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

    class Enumerator : public Node
    {
    private:
        const std::string name_;
        NodePtr value_;

    public:
        Enumerator(const std::string &name, NodePtr value)
            : name_(name), value_(std::move(value)) {}

        const std::string &GetName() const { return name_; }
        const NodePtr &GetValue() const { return value_; }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
