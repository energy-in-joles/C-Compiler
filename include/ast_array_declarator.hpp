#pragma once
#include "ast_node.hpp"

namespace ast
{
    class ArrayDeclarator : public Node
    {
    private:
        NodePtr identifier_;
        NodePtr size_;

    public:
        ArrayDeclarator(NodePtr identifier, NodePtr size)
            : identifier_(std::move(identifier)), size_(std::move(size)) {}

        std::string GetID() const override;
        bool IsFunction() const override;
        bool IsArray() const override;
        int GetArraySize(Context &context) const override;

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
