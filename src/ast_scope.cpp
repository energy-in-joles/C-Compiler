#include "ast_scope.hpp"
#include <cerrno>
#include <vector>

namespace ast
{

    void Scope::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        // TODO: check if statements is empty

        // Create new scope
        if (!context.InGlobalScope())
        {
            FunctionContext &function_context = context.GetCurrentFunctionContext();
            function_context.EnterScope();
        }

        // Execute any instructions nested within scope and emit riscv
        statements_.EmitRISC(stream, context, destReg, type);

        // Destroy scope
        if (!context.InGlobalScope())
        {
            FunctionContext &function_context = context.GetCurrentFunctionContext();
            function_context.ExitScope();
        }
    }

    void Scope::Print(std::ostream &stream) const
    {
        for (auto const &statement : statements_)
        {
            statement->Print(stream);
        }
    }
}
