#include "ast_array_index.hpp"
#include <typeinfo>
#include <any>
#include <string>
#include <iostream>

namespace ast
{
    void ArrayIndex::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        std::string array_name = GetID();

        // Calculate element offset
        int indexReg = context.AssignRegister(TypeSpecifier::INT);
        index_->EmitRISC(stream, context, indexReg, TypeSpecifier::INT);

        int tmpReg = context.AssignRegister(TypeSpecifier::INT);
        stream << "li " << context.GetRegString(tmpReg) << ", " << context.GetPointerOffset(GetID(), GetPointerDepth()) << "\n";
        stream << "mul " << context.GetRegString(indexReg) << ", " << context.GetRegString(indexReg) << ", " << context.GetRegString(tmpReg) << "\n";

        // handle pointer indexing as well
        if (array_id_->IsPointer(context, true))
        {
            array_id_->EmitRISC(stream, context, tmpReg, TypeSpecifier::INT);
        }
        else
        {
            if (context.VariableIsLocal(array_name))
            {
                const FunctionVariable var = context.GetCurrentFunctionContext().GetFunctionVariableInfo(array_name);
                stream << "addi " << context.GetRegString(tmpReg) << ", s0, "
                       << var.offset << "\n";
            }
            else
            {
                stream << "lui " << context.GetRegString(tmpReg) << ", %hi(" << array_name << ")\n";
                stream << "addi " << context.GetRegString(tmpReg) << ", " << context.GetRegString(tmpReg) << ", %lo(" << array_name << ")\n";
            }
        }

        // Add index offset
        stream << "add " << context.GetRegString(tmpReg) << ", "
               << context.GetRegString(tmpReg) << ", "
               << context.GetRegString(indexReg) << "\n";

        // Load value
        stream << GetLoadOp(type) << " " << context.GetRegString(destReg) << ", 0("
               << context.GetRegString(tmpReg) << ")\n";

        context.FreeRegister(indexReg);
        context.FreeRegister(tmpReg);
    }

    void ArrayIndex::Print(std::ostream &stream) const
    {
        array_id_->Print(stream);
        stream << "[";
        index_->Print(stream);
        stream << "]";
    }

    std::string ArrayIndex::GetID() const
    {
        return array_id_->GetID();
    }

    TypeSpecifier ArrayIndex::GetType(Context &context) const
    {
        return array_id_->GetType(context);
    }

    bool ArrayIndex::IsFunction() const
    {
        return false;
    }

    bool ArrayIndex::IsArray() const
    {
        return true;
    }

    bool ArrayIndex::IsPointer(Context &context, const bool has_been_declared) const
    {
        return array_id_->IsPointer(context, has_been_declared);
    }

    int ArrayIndex::GetPointerDepth() const
    {
        return array_id_->GetPointerDepth();
    }

    void ArrayIndex::EmitIndexOffset(std::ostream &stream, Context &context, int destReg) const
    {
        int srcReg = context.AssignRegister(TypeSpecifier::INT);
        int tmpOffsetReg = context.AssignRegister(TypeSpecifier::INT);
        index_->EmitRISC(stream, context, srcReg, TypeSpecifier::INT);
        stream << "li " << context.GetRegString(tmpOffsetReg) << ", " << context.GetPointerOffset(GetID(), GetPointerDepth()) << "\n";
        stream << "mul " << context.GetRegString(srcReg) << ", " << context.GetRegString(srcReg) << ", " << context.GetRegString(tmpOffsetReg) << "\n";
        stream << "add " << context.GetRegString(destReg) << "," << context.GetRegString(destReg)
               << "," << context.GetRegString(srcReg) << "\n";
        context.FreeRegister(srcReg);
        context.FreeRegister(tmpOffsetReg);
    }

} // namespace ast
