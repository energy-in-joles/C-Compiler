#pragma once

#include "ast_node.hpp"

namespace ast
{

    class SwitchStatement : public Node
    {
    private:
        NodePtr expression_;
        NodePtr statement_;

    public:
        SwitchStatement(NodePtr expression, NodePtr statement)
            : expression_(std::move(expression)),
              statement_(std::move(statement)) {};

        const Node *GetExpression() const { return expression_.get(); }
        const Node *GetStatement() const { return statement_.get(); }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

    class CaseStatement : public Node
    {
    private:
        NodePtr condition_;
        NodePtr body_;

    public:
        CaseStatement(NodePtr condition, NodePtr body)
            : condition_(std::move(condition)),
              body_(std::move(body)) {};

        const Node *GetCondition() const { return condition_.get(); }
        const Node *GetBody() const { return body_.get(); }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };

    class DefaultStatement : public Node
    {
    private:
        NodePtr body_;

    public:
        DefaultStatement(NodePtr body)
            : body_(std::move(body)) {};

        const Node *GetBody() const { return body_.get(); }

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
    };
}
