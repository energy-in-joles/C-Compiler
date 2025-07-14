#include "ast_while.hpp"
#include <ostream>

namespace ast
{

    void WhileLoop::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string startLabel = context.GenerateUniqueLabel("while_start");
        std::string endLabel = context.GenerateUniqueLabel("while_end");

        context.NewLoopContext(ControlFlowType::WHILE, startLabel, endLabel, "");

        stream << startLabel << ":" << std::endl;
        int tmpConditionReg = context.AssignRegister(TypeSpecifier::INT);
        condition_->EmitRISC(stream, context, tmpConditionReg, TypeSpecifier::INT);
        stream << "beq " << context.GetRegString(tmpConditionReg) << ", zero, " << endLabel << std::endl;
        body_->EmitRISC(stream, context, destReg, type);
        stream << "j " << startLabel << std::endl;
        stream << endLabel << ":" << std::endl;

        context.FreeRegister(tmpConditionReg);
        context.EndLoopContext();
    }

    void WhileLoop::Print(std::ostream &stream) const
    {
        stream << "while(";
        condition_->Print(stream);
        stream << ") {" << std::endl;
        body_->Print(stream);
        stream << "}" << std::endl;
    };
}
