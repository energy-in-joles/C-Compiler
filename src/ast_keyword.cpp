#include "ast_keyword.hpp"
#include <ostream>

namespace ast
{
    // ---------- CONTINUE ----------
    void ContinueStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)destReg;
        (void)type;

        // jump to start of loop
        if (context.GetCurrentLoopContext().type == ControlFlowType::FOR)
        {
            // jump to the update label for for loops
            stream << "j " << context.GetUpdateLabel() << std::endl;
        }
        else
        {
            // jump to the start label for other loops
            stream << "j " << context.GetStartLabel() << std::endl;
        }
    }

    void ContinueStatement::Print(std::ostream &stream) const
    {
        stream << "continue;" << std::endl;
    };

    // ---------- BREAK ----------
    void BreakStatement::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)destReg;
        (void)type;
        // jump to end of loop
        stream << "j " << context.GetEndLabel() << std::endl;
    }

    void BreakStatement::Print(std::ostream &stream) const
    {
        stream << "break;" << std::endl;
    };
}
