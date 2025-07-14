#pragma once

#include "ast_node.hpp"

namespace ast
{

    class PointerDeclarator : public Node
    {
    private:
        NodePtr declarator_;
        int pointer_depth_;

    public:
        PointerDeclarator(int pointer_depth, NodePtr declarator) : declarator_(std::move(declarator)), pointer_depth_(pointer_depth) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
        int GetPointerDepth() const override;

        bool IsArray() const override;
        int GetArraySize(Context &context) const override;

        bool IsFunction() const override;
        void StoreFunctionInfo(TypeSpecifier return_type, Context &context) const; // when wrapping a function
    };

} // namespace ast
