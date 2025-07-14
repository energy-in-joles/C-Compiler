#include "ast_jump_statement.hpp"
#include <thread>

namespace ast
{

    void ReturnStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        if (expression_ != nullptr)
        {
            TypeSpecifier ret_type = expression_->GetType(context);
            expression_->EmitRISC(stream, context, destReg, ret_type);
            if (ret_type == TypeSpecifier::INT || ret_type == TypeSpecifier::CHAR || ret_type == TypeSpecifier::UNSIGNED)
            {
                stream << "mv a0" << ", " << context.GetRegString(destReg) << std::endl;
            }
            else if (ret_type == TypeSpecifier::FLOAT)
            {
                stream << "fmv.s fa0" << ", " << context.GetRegString(destReg) << std::endl;
            }
            else if (ret_type == TypeSpecifier::DOUBLE)
            {
                stream << "fmv.d fa0" << ", " << context.GetRegString(destReg) << std::endl;
            }
            else
            {
                std::runtime_error("ReturnStatement: Invalid Type.");
            }
        }
        std::string functionLabel = context.GetCurrentFunctionContext().GetEndLabel();
        stream << "j " << functionLabel << std::endl;
    }

    void ReturnStatement::Print(std::ostream &stream) const
    {
        stream << "return";
        if (expression_ != nullptr)
        {
            stream << " ";
            expression_->Print(stream);
        }
        stream << ";" << std::endl;
    }
}
