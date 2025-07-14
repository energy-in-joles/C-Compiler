#include "ast_identifier.hpp"
#include <stdexcept>

namespace ast
{
    void Identifier::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        if (context.InEnum(identifier_))
        {
            int value = context.GetEnumValue(identifier_);
            stream << "li " << context.GetRegString(destReg) << ", " << value << std::endl;
            return;
        }
        else if (context.VariableIsLocal(identifier_))
        {
            type = (IsPointer(context, true)) ? TypeSpecifier::INT : type;
            int offset = context.GetCurrentFunctionContext().GetFunctionVariableInfo(identifier_).offset;
            switch (type)
            {
            case TypeSpecifier::INT:
                stream << "lw " << context.GetRegString(destReg) << ", " << offset << "(s0)" << std::endl;
                break;
            case TypeSpecifier::CHAR:
                stream << "lbu " << context.GetRegString(destReg) << ", " << offset << "(s0)" << std::endl;
                break;
            case TypeSpecifier::FLOAT:
                stream << "flw " << context.GetRegString(destReg) << ", " << offset << "(s0)" << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fld " << context.GetRegString(destReg) << ", " << offset << "(s0)" << std::endl;
                break;
            default:
                throw std::runtime_error("Identifier Function: Not all types implemented.");
            }
        }
        else
        {
            int srcReg = context.AssignRegister(TypeSpecifier::INT);
            stream << "lui " << context.GetRegString(srcReg) << ", %hi(" << identifier_ << ")" << std::endl;
            switch (type)
            {
            case TypeSpecifier::INT:
                stream << "lw " << context.GetRegString(destReg) << ", %lo(" << identifier_ << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
                break;
            case TypeSpecifier::CHAR:
                stream << "lbu " << context.GetRegString(destReg) << ", %lo(" << identifier_ << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
                break;
            case TypeSpecifier::FLOAT:
                stream << "flw " << context.GetRegString(destReg) << ", %lo(" << identifier_ << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
                break;
            case TypeSpecifier::DOUBLE:
                stream << "fld " << context.GetRegString(destReg) << ", %lo(" << identifier_ << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
                break;
            default:
                throw std::runtime_error("Identifier Global: Not all types implemented.");
            }
            context.FreeRegister(srcReg);
        }
    }

    void Identifier::Print(std::ostream &stream) const
    {
        stream << identifier_;
    }

    std::string Identifier::GetID() const
    {
        return identifier_;
    }

    TypeSpecifier Identifier::GetType(Context &context) const
    {
        if (context.VariableIsLocal(identifier_))
        {
            return context.GetCurrentFunctionContext().GetFunctionVariableInfo(identifier_).type;
        }
        else
        {
            return context.GetGlobalVariableInfo(identifier_).type;
        }
    }

    bool Identifier::IsPointer(Context &context, const bool has_been_declared) const
    {
        if (!has_been_declared)
        {
            return false;
        }
        if (context.VariableIsLocal(identifier_))
        {
            return context.GetCurrentFunctionContext().GetFunctionVariableInfo(identifier_).is_pointer;
        }
        else
        {
            return context.GetGlobalVariableInfo(identifier_).is_pointer;
        }
    }
}
