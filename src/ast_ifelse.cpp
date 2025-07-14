#include "ast_ifelse.hpp"
#include <ostream>

namespace ast
{

    void IfStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        int tmpConditionReg = context.AssignRegister(TypeSpecifier::INT);

        std::string endLabel = context.GenerateUniqueLabel("end");
        std::string elseLabel = (elseBody_) ? context.GenerateUniqueLabel("else") : "";

        condition_->EmitRISC(stream, context, tmpConditionReg, TypeSpecifier::INT);

        if (elseBody_)
        {
            stream << "beq " << context.GetRegString(tmpConditionReg) << ", zero, " << elseLabel << std::endl;
        }
        else
        {
            stream << "beq " << context.GetRegString(tmpConditionReg) << ", zero, " << endLabel << std::endl;
        }

        context.FreeRegister(tmpConditionReg);

        ifBody_->EmitRISC(stream, context, destReg, type);

        if (elseBody_)
        {
            stream << "j " << endLabel << std::endl;
            stream << elseLabel << ":" << std::endl;
            elseBody_->EmitRISC(stream, context, destReg, type);
        }

        stream << endLabel << ":" << std::endl;
    }

    void IfStatement::Print(std::ostream &stream) const
    {
        stream << "if(";
        condition_->Print(stream);
        stream << ") {" << std::endl;
        ifBody_->Print(stream);
        stream << "}";

        if (elseBody_)
        {
            stream << "else {" << std::endl;
            elseBody_->Print(stream);
            stream << "}" << std::endl;
        }
        stream << std::endl;
    };

    TypeSpecifier IfStatement::GetType(Context &context) const
    {
        (void)context; // Unused
        return ifBody_->GetType(context);
    }
}
