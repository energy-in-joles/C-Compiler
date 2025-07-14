#include "ast_node.hpp"

namespace ast
{

    void NodeList::PushBack(NodePtr item)
    {
        nodes_.push_back(std::move(item));
    }

    void NodeList::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        for (const auto &node : nodes_)
        {
            if (node == nullptr)
            {
                continue;
            }
            TypeSpecifier node_type = node->GetType(context);
            // if parent node is void, we assign register based on child node type
            if (type == TypeSpecifier::VOID && node_type != TypeSpecifier::VOID)
            {
                int tmpDestReg = context.AssignRegister(node_type);
                node->EmitRISC(stream, context, tmpDestReg, node_type);
                context.FreeRegister(tmpDestReg);
            }
            // else enforce parent node type
            else
            {
                node->EmitRISC(stream, context, destReg, type);
            }
        }
    }

    void NodeList::Print(std::ostream &stream) const
    {
        for (const auto &node : nodes_)
        {
            if (node == nullptr)
            {
                continue;
            }
            node->Print(stream);
        }
    }

    std::vector<std::string> NodeList::GetIDs() const
    {
        std::vector<std::string> ids;
        for (const auto &node : nodes_)
        {
            if (node == nullptr)
            {
                continue;
            }
            ids.push_back(node->GetID());
        }
        return ids;
    }

    std::vector<ParamInfo> NodeList::GetParams(Context &context) const
    {
        std::vector<ParamInfo> params;
        for (const auto &node : nodes_)
        {
            if (node == nullptr)
            {
                continue;
            }
            TypeSpecifier type = node->GetType(context);
            bool is_pointer = node->IsPointer(context, false);
            int pointer_depth = node->GetPointerDepth();
            std::string name = node->GetID();
            params.push_back({name, type, is_pointer, pointer_depth});
        }
        return params;
    }

    int NodeList::GetArraySize(Context &context) const
    {
        (void)context;
        return Size();
    }
}
