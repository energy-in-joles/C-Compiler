#include "ast_declaration.hpp"
#include <unordered_map>

namespace ast
{
    void Declaration::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        (void)destReg; // destReg is invalid. It's a trap!
        // only function definition and this case ignore destReg entirely.
        const NodeList &node_list = dynamic_cast<const NodeList &>(*declarator_list_);
        if (context.InGlobalScope())
        {
            for (const auto &node : node_list)
            {
                // NB: this also adds functions to the variable table.
                // This is fine because global names are unique anyway.
                // this should be fine with strings
                if (node->IsArray())
                {
                    context.AddGlobalArray(node->GetID(), node->GetArraySize(context), declaration_specifiers_);
                }
                else
                {
                    context.AddGlobalVariable(node->GetID(), declaration_specifiers_, node->IsPointer(context, false), node->GetPointerDepth());
                }
            }
        }
        else
        {
            for (const auto &node : node_list)
            {
                if (node->IsArray())
                {
                    context.GetCurrentFunctionContext().AddArray(node->GetID(), node->GetArraySize(context), declaration_specifiers_);
                }
                else
                {
                    context.GetCurrentFunctionContext().AddFunctionVariable(node->GetID(), declaration_specifiers_, node->IsPointer(context, false), node->GetPointerDepth());
                }
            }
        }
        int srcReg = context.AssignRegister(declaration_specifiers_);
        declarator_list_->EmitRISC(stream, context, srcReg, declaration_specifiers_);
        context.FreeRegister(srcReg);
    }

    void Declaration::Print(std::ostream &stream) const
    {
        stream << declaration_specifiers_ << " ";
        declarator_list_->Print(stream);
        // TODO: this is a big bugged because we don't print commas currently
        stream << ";" << std::endl;
    }
}
