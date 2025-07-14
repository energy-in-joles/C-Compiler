#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <any>

#include "ast_type_specifier.hpp"
#include "ast_context.hpp"

namespace ast
{

    class Node
    {
    public:
        virtual ~Node() {}
        virtual void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const = 0;
        virtual void Print(std::ostream &stream) const = 0;
        virtual std::string GetID() const { return ""; };
        virtual std::vector<std::string> GetIDs() const { throw std::runtime_error("GetIDs not implemented"); };
        virtual bool IsFunction() const { return false; }; // used to discern DirectDeclarator and Identifier

        // --- POINTERS
        virtual bool IsPointer(Context &context, const bool has_been_declared) const
        {
            (void)context;
            (void)has_been_declared;
            return false;
        }; // used to discern PointerDeclarator and Identifier
        virtual int GetPointerDepth() const { return 0; }; // used to get the depth of a pointer

        // --- ARRAYS
        virtual bool IsArray() const { return false; }; // used to discern ArrayDeclarator and Identifier
        virtual int GetArraySize(Context &context) const
        {
            (void)context;
            return 0;
        }; // used to get the size of an array

        virtual std::any GetValue(TypeSpecifier type) const
        {
            (void)type;
            throw std::runtime_error("GetValue not implemented");
        }; // used for compile time evaluation (global variables)
        virtual TypeSpecifier GetType(Context &context) const
        {
            (void)context;
            return TypeSpecifier::VOID;
        };
        virtual std::vector<ParamInfo> GetParams(Context &context) const
        {
            (void)context;
            throw std::runtime_error("GetParams not implemented");
        };
    };

    // If you don't feel comfortable using std::unique_ptr, you can switch NodePtr to be defined
    // as a raw pointer instead here and your project should still compile, although you'll need
    // to add destructors to avoid leaking memory
    // (and get rid of the now unnecessary std::move-s)
    using NodePtr = std::unique_ptr<const Node>;

    class NodeList : public Node
    {
    private:
        std::vector<NodePtr> nodes_;

    public:
        NodeList(NodePtr first_node) { nodes_.push_back(std::move(first_node)); }

        using iterator = std::vector<NodePtr>::iterator;
        using const_iterator = std::vector<NodePtr>::const_iterator;

        void PushBack(NodePtr item);
        virtual void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        virtual void Print(std::ostream &stream) const override;
        virtual std::vector<std::string> GetIDs() const;
        virtual std::vector<ParamInfo> GetParams(Context &context) const;
        virtual int GetArraySize(Context &context) const override;

        // The following methods are needed to iterate over the nodes in the NodeList
        iterator begin() { return nodes_.begin(); }
        iterator end() { return nodes_.end(); }
        const_iterator begin() const { return nodes_.begin(); }
        const_iterator end() const { return nodes_.end(); }
        size_t Size() const { return nodes_.size(); }
    };

} // namespace ast
