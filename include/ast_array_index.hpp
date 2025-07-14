#pragma once
#include "ast_node.hpp"

namespace ast
{
    class ArrayIndex : public Node
    {
    private:
        NodePtr array_id_;
        NodePtr index_;

    public:
        ArrayIndex(NodePtr arrayid, NodePtr index)
            : array_id_(std::move(arrayid)), index_(std::move(index)) {}

        std::string GetID() const override;
        bool IsFunction() const override;
        TypeSpecifier GetType(Context &context) const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
        bool IsArray() const override;
        int GetPointerDepth() const override;
        void EmitIndexOffset(std::ostream &stream, Context &context, int destReg) const;

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

} // namespace ast
