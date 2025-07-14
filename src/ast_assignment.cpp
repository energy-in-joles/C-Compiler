#include "ast_assignment.hpp"
#include "ast_array_index.hpp"
#include <stdexcept>

namespace ast
{

    const std::unordered_map<std::string, std::string> assignmentStringsBase = {
        {"=", "NIL"},   // Assignment (we don't use any ops for this)
        {"*=", "mul"},  // MUL_ASSIGN
        {"/=", "div"},  // DIV_ASSIGN
        {"%=", "rem"},  // MOD_ASSIGN
        {"+=", "add"},  // ADD_ASSIGN
        {"-=", "sub"},  // SUB_ASSIGN
        {"<<=", "sll"}, // LEFT_ASSIGN
        {">>=", "srl"}, // RIGHT_ASSIGN
        {"&=", "and"},  // AND_ASSIGN
        {"^=", "xor"},  // XOR_ASSIGN
        {"|=", "or"},   // OR_ASSIGN
    };

    void Assignment::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        (void)destReg; // don't use destReg because destination can be a float register

        TypeSpecifier destination_type;
        int tmpDestReg;

        // ------ LOADING THE DESTINATION REGISTER ---------
        if (destination_->IsPointer(context, true))
        {
            destination_type = TypeSpecifier::INT;
        }
        else if (context.VariableIsLocal(destination_->GetID()))
        {
            destination_type = context.GetCurrentFunctionContext().GetFunctionVariableInfo(destination_->GetID()).type;
        }
        else
        {
            destination_type = context.GetGlobalVariableInfo(destination_->GetID()).type;
        }
        tmpDestReg = context.AssignRegister(destination_type);

        // ------ EXECUTING THE ASSIGNMENT ---------
        if (assignment_str_ == "=")
        {
            source_->EmitRISC(stream, context, tmpDestReg, destination_type);
        }
        else
        {
            int srcReg = context.AssignRegister(destination_type);
            source_->EmitRISC(stream, context, srcReg, destination_type);
            destination_->EmitRISC(stream, context, tmpDestReg, destination_type);
            // if destination is a pointer, we need to multiply by WORD_SIZE
            if (destination_->IsPointer(context, true))
            {
                // we assume valid pointer operations and that pointer is always an int
                int tmpIndexReg = context.AssignRegister(TypeSpecifier::INT);
                stream << "li " << context.GetRegString(tmpIndexReg) << "," << context.GetPointerOffset(destination_->GetID(), destination_->GetPointerDepth()) << std::endl;
                stream << "mul " << context.GetRegString(srcReg) << "," << context.GetRegString(srcReg) << "," << context.GetRegString(tmpIndexReg) << std::endl;
                context.FreeRegister(tmpIndexReg);
            }
            std::string assignment_string = GetArithmeticOp(destination_type, assignmentStringsBase.at(assignment_str_));
            stream << assignment_string << " " << context.GetRegString(tmpDestReg) << "," << context.GetRegString(tmpDestReg) << "," << context.GetRegString(srcReg) << std::endl;
            context.FreeRegister(srcReg);
        }

        // ------ STORING THE RESULT ---------
        if (context.VariableIsLocal(destination_->GetID()))
        {
            FunctionVariable dest_var_info = context.GetCurrentFunctionContext().GetFunctionVariableInfo(destination_->GetID());

            // for arrays and pointer indexing
            if (destination_->IsArray())
            {
                int tmpMemReg = context.AssignRegister(TypeSpecifier::INT);
                // deal with pointer indexing
                if (destination_->IsPointer(context, true))
                {
                    destination_->EmitRISC(stream, context, tmpMemReg, TypeSpecifier::INT);
                }
                // deal with array indexing
                else
                {
                    stream << "addi " << context.GetRegString(tmpMemReg) << ", s0, " << dest_var_info.offset << "\n";
                }
                dynamic_cast<const ArrayIndex *>(destination_.get())->EmitIndexOffset(stream, context, tmpMemReg);
                stream << GetStoreOp(destination_type) << " " << context.GetRegString(tmpDestReg) << ",0(" << context.GetRegString(tmpMemReg) << ")" << std::endl;
            }
            // if we are derefencing a pointer
            else if (destination_->GetPointerDepth() > 0)
            {
                int tmpMemReg = context.AssignRegister(TypeSpecifier::INT);
                EmitPointerDereference(stream, context, tmpMemReg);
                // we store either a pointer at a lower depth or the base value the pointer was pointing to
                stream << GetStoreOp(destination_type) << " " << context.GetRegString(tmpDestReg) << ",0(" << context.GetRegString(tmpMemReg) << ")" << std::endl;
                context.FreeRegister(tmpMemReg);
            }
            // else handle normal registers or pointers that are not dereferenced at all
            else
            {
                stream << GetStoreOp(destination_type) << " " << context.GetRegString(tmpDestReg) << "," << dest_var_info.offset << "(s0)" << std::endl;
            }
        }
        else
        {
            int tmpMemReg = context.AssignRegister(TypeSpecifier::INT); // for loading memory location
            if (destination_->GetPointerDepth() > 0)
            {
                EmitPointerDereference(stream, context, tmpMemReg);
                stream << GetStoreOp(destination_type) << " " << context.GetRegString(tmpDestReg) << ",0(" << context.GetRegString(tmpMemReg) << ")" << std::endl;
            }
            else
            {
                stream << "lui " << context.GetRegString(tmpMemReg) << ",%hi(" << destination_->GetID() << ")" << std::endl;
                stream << GetStoreOp(destination_type) << " " << context.GetRegString(tmpDestReg) << ",%lo(" << destination_->GetID() << ")(" << context.GetRegString(tmpMemReg) << ")" << std::endl;
            }
            context.FreeRegister(tmpMemReg);
        }
        context.FreeRegister(tmpDestReg);
    }

    // we use this for LHS pointer dereferencing to retrieve the memory location to store to
    void Assignment::EmitPointerDereference(std::ostream &stream, Context &context, int destReg) const
    {
        if (context.VariableIsLocal(destination_->GetID()))
        {
            stream << "lw " << context.GetRegString(destReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(destination_->GetID()).offset << "(s0)" << std::endl;
        }
        else
        {
            stream << "lui " << context.GetRegString(destReg) << ",%hi(" << destination_->GetID() << ")" << std::endl;
            stream << "addi " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << ",%lo(" << destination_->GetID() << ")" << std::endl;
        }

        for (int i = 0; i < destination_->GetPointerDepth() - 1; i++)
        {
            stream << "lw " << context.GetRegString(destReg) << ",0(" << context.GetRegString(destReg) << ")" << std::endl;
        }
    }

    void Assignment::Print(std::ostream &stream) const
    {
        destination_->Print(stream);
        stream << " " << assignment_str_ << " ";
        source_->Print(stream);
        stream << std::endl;
    }
}
