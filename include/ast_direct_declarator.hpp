#pragma once

#include "ast_node.hpp"

namespace ast
{

    class DirectDeclarator : public Node
    {
    private:
        NodePtr identifier_;
        NodePtr parameter_list_;

    public:
        DirectDeclarator(NodePtr identifier, NodePtr parameter_list) : identifier_(std::move(identifier)), parameter_list_(std::move(parameter_list)) {};
        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;
        bool IsFunction() const override { return true; };
        void StoreFunctionInfo(TypeSpecifier return_type, Context &context) const;
    };

} // namespace ast
