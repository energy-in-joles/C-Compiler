#include "ast_function_definition.hpp"
#include "ast_direct_declarator.hpp"
#include "ast_pointer_declarator.hpp"
#include <vector>
#include <sstream>

std::ostringstream buffer;
std::terminate_handler defaultTerminate = nullptr;
ast::Context *context_ptr = nullptr;

namespace ast
{
    void customTerminate()
    {
        std::cerr << std::endl;
        std::cerr << "Exception ocurred: Dumping function stream contents:\n";
        std::cerr << buffer.str() << std::endl; // Dump log stream

        if (context_ptr != nullptr)
        {
            std::cerr << context_ptr->PrintContext().str() << std::endl; // Dump context
        }

        try
        {
            throw; // Re-throw the current exception
        }
        catch (const std::runtime_error &e)
        {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Exception occurred: " << e.what() << std::endl;
        }

        std::abort();
    }

    void FunctionDefinition::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        context_ptr = &context;
        buffer.str(""); // clear buffer
        (void)destReg;  // ignore

        std::string function_name_ = declarator_->GetID();

        stream << ".text" << std::endl;
        stream << ".globl " << function_name_ << std::endl;
        stream << function_name_ << ":" << std::endl;

        context.CreateNewFunctionScope(function_name_);
        FunctionContext &function_context = context.GetCurrentFunctionContext();

        // start logging
        defaultTerminate = std::set_terminate(customTerminate);

        // Store function info and handle params being passed
        if (declarator_->IsPointer(context, false))
        {
            const PointerDeclarator &pointer_declarator = dynamic_cast<const PointerDeclarator &>(*declarator_);
            pointer_declarator.StoreFunctionInfo(declaration_specifiers_, context);
            declarator_->EmitRISC(buffer, context, destReg, TypeSpecifier::INT);
        }
        else
        {
            const DirectDeclarator &func_declarator = dynamic_cast<const DirectDeclarator &>(*declarator_);
            func_declarator.StoreFunctionInfo(declaration_specifiers_, context);
            declarator_->EmitRISC(buffer, context, destReg, type);
        }

        // EmitRISC for the function body
        // if void, then just retrieve a GPR
        int retReg = context.AssignRegister((declaration_specifiers_ == TypeSpecifier::VOID) ? TypeSpecifier::INT : declaration_specifiers_);

        // pass void type down to compound:
        // indicate we don't have an expected destreg for any of the statements (except return)
        compound_statement_->EmitRISC(buffer, context, retReg, type);
        context.FreeRegister(retReg);

        buffer << function_context.GetEndLabel() << ":" << std::endl;

        // note that this stack size may be different from the defaultStackSize, as it may have been increased
        // during the function body
        buffer << "lw ra," << (function_context.GetStackSize() - 2 * WORD_SIZE) << "(sp)" << std::endl;
        buffer << "lw s0," << (function_context.GetStackSize() - 3 * WORD_SIZE) << "(sp)" << std::endl;
        buffer << "addi sp,sp," << function_context.GetStackSize() << std::endl;
        buffer << "ret" << std::endl;

        // stream the stack setup
        stream << "addi sp,sp,-" << function_context.GetStackSize() << std::endl;
        stream << "sw ra," << (function_context.GetStackSize() - 2 * WORD_SIZE) << "(sp)" << std::endl;
        stream << "sw s0," << (function_context.GetStackSize() - 3 * WORD_SIZE) << "(sp)" << std::endl;
        stream << "addi s0,sp," << function_context.GetStackSize() << std::endl;

        // stream the rest of the function
        stream << buffer.str();

        context.DestroyFunctionScope();
    }

    void FunctionDefinition::Print(std::ostream &stream) const
    {
        stream << declaration_specifiers_ << " ";

        declarator_->Print(stream);
        stream << "() {" << std::endl;

        if (compound_statement_ != nullptr)
        {
            compound_statement_->Print(stream);
        }
        stream << "}" << std::endl;
    }

}
