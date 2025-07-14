#include "ast_for.hpp"
#include <ostream>

namespace ast
{

    void ForStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        // Generate unique labels for the for loop
        std::string startLabel = context.GenerateUniqueLabel("start_for");
        std::string updateLabel = context.GenerateUniqueLabel("update_for");
        std::string endLabel = context.GenerateUniqueLabel("end_for");

        context.NewLoopContext(ControlFlowType::FOR, startLabel, endLabel, updateLabel);

        if (init_assignment_ != nullptr)
        {
            init_assignment_->EmitRISC(stream, context, destReg, type);
        }

        stream << startLabel << ":" << std::endl;

        if (condition_ != nullptr)
        {
            int tmpConditionReg = context.AssignRegister(TypeSpecifier::INT);
            condition_->EmitRISC(stream, context, tmpConditionReg, TypeSpecifier::INT);
            stream << "beq " << context.GetRegString(tmpConditionReg) << ", zero, " << endLabel << std::endl;
            context.FreeRegister(tmpConditionReg);
        }

        if (for_body_ != nullptr)
        {
            for_body_->EmitRISC(stream, context, destReg, type);
        }

        stream << updateLabel << ":" << std::endl;

        if (update_assignment_ != nullptr)
        {
            // for assignment expressions: x=x+1
            if (update_assignment_->GetType(context) == TypeSpecifier::VOID)
            {
                update_assignment_->EmitRISC(stream, context, destReg, type);
            }
            // for case where update assignment is x++ (increment/decrement)
            else
            {
                int updateReg = context.AssignRegister(update_assignment_->GetType(context));
                update_assignment_->EmitRISC(stream, context, updateReg, update_assignment_->GetType(context));
                context.FreeRegister(updateReg);
            }
        }

        stream << "j " << startLabel << std::endl;

        stream << endLabel << ":" << std::endl;

        context.EndLoopContext();
    }

    void ForStatement::Print(std::ostream &stream) const
    {
        stream << "for(";
        init_assignment_->Print(stream);
        stream << "; ";
        condition_->Print(stream);
        stream << "; ";
        update_assignment_->Print(stream);
        stream << ") {" << std::endl;
        for_body_->Print(stream);
        stream << "}" << std::endl;
    };
}
