#pragma once
#include "ast_node.hpp"

namespace ast
{
    std::vector<std::string> SaveUsedRegisters(Context &context, std::ostream &stream);
    void RestoreUsedRegisters(Context &context, std::ostream &stream, const std::vector<std::string> &saved_registers);

    class FunctionCall : public Node
    {
    private:
        NodePtr postfix_expression_;
        NodePtr argument_expression_list_;

    public:
        FunctionCall(NodePtr postfix_expression, NodePtr argument_expression_list)
            : postfix_expression_(std::move(postfix_expression)), argument_expression_list_(std::move(argument_expression_list)) {};

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

} // namespace ast
