#pragma once

#include "ast_node.hpp"

namespace ast
{
    class InitDeclarator : public Node
    {
    private:
        NodePtr declarator_;
        NodePtr initializer_;
        void EmitLocalDefinition(std::ostream &stream, std::string srcRegStr, int offset, TypeSpecifier type) const;
        void EmitGlobalDefinition(std::ostream &stream, std::any value, TypeSpecifier type, std::string id) const;

    public:
        InitDeclarator(NodePtr declarator, NodePtr initializer)
            : declarator_(std::move(declarator)),
              initializer_(std::move(initializer)) {};

        bool IsFunction() const override;
        bool IsPointer(Context &context, const bool has_been_declared) const override;
        bool IsArray() const override;
        int GetPointerDepth() const override;
        int GetArraySize(Context &context) const override;

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;
    };

} // namespace ast
