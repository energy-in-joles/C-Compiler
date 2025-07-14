#include "ast_increment.hpp"
#include "unordered_map"

namespace ast
{
    std::string IncAndDecOp::GetAddOrSubOp(TypeSpecifier type) const
    {
        if (addi_value_ == "1")
        {
            switch (type)
            {
            case TypeSpecifier::INT:
                return "addi";
            case TypeSpecifier::FLOAT:
                return "fadd.s";
            case TypeSpecifier::DOUBLE:
                return "fadd.d";
            default:
                throw std::runtime_error("Invalid type for IncAndDecOp");
            }
        }
        else
        {
            switch (type)
            {
            case TypeSpecifier::INT:
                return "subi";
            case TypeSpecifier::FLOAT:
                return "fsub.s";
            case TypeSpecifier::DOUBLE:
                return "fsub.d";
            default:
                throw std::runtime_error("Invalid type for IncAndDecOp");
            }
        }
    }

    void IncAndDecOp::SetAddiValue(char op_symbol_)
    {
        switch (op_symbol_)
        {
        case '+':
            addi_value_ = "1";
            break;
        case '-':
            addi_value_ = "-1";
            break;
        default:
            throw std::runtime_error("Invalid operator for IncAndDecOp");
        }
    }

    TypeSpecifier IncAndDecOp::GetType(Context &context) const
    {
        return expression_->GetType(context);
    }

    void PostIncAndDecOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        // TODO: not sure if this works for array as well
        std::string expr_name = expression_->GetID();
        int tempReg = context.AssignRegister(type);
        expression_->EmitRISC(stream, context, destReg, type);

        if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE)
        {
            int lc_n = context.AddFloatLiteralConstant(1.0, type);
            int srcMemReg = context.AssignRegister(TypeSpecifier::INT);
            stream << "lui " << context.GetRegString(srcMemReg) << ", %hi(.LC" << lc_n << ")" << std::endl;
            stream << GetLoadOp(type) << " " << context.GetRegString(tempReg) << ",%lo(.LC" << lc_n << ")(" << context.GetRegString(srcMemReg) << ")" << std::endl;
            stream << GetAddOrSubOp(type) << " " << context.GetRegString(tempReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(tempReg) << std::endl;
            if (context.VariableIsLocal(expr_name))
            {
                stream << GetStoreOp(type) << " " << context.GetRegString(tempReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(expr_name).offset << "(s0)" << std::endl;
            }
            else
            {
                stream << "lui " << context.GetRegString(srcMemReg) << ",%hi(" << expr_name << ")" << std::endl;
                stream << GetStoreOp(type) << " " << context.GetRegString(tempReg) << ",%lo(" << expr_name << ")(" << context.GetRegString(srcMemReg) << ")" << std::endl;
            }
            context.FreeRegister(srcMemReg);
        }
        else
        {
            stream << "addi " << context.GetRegString(tempReg) << "," << context.GetRegString(destReg) << "," << addi_value_ << std::endl;
            if (context.VariableIsLocal(expr_name))
            {
                stream << "sw " << context.GetRegString(tempReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(expr_name).offset << "(s0)" << std::endl;
            }
            else
            {
                int tempReg2 = context.AssignRegister(TypeSpecifier::INT);
                stream << "lui " << context.GetRegString(tempReg2) << ",%hi(" << expr_name << ")" << std::endl;
                stream << "sw " << context.GetRegString(tempReg) << ",%lo(" << expr_name << ")(" << context.GetRegString(tempReg2) << ")" << std::endl;
                context.FreeRegister(tempReg2);
            }
            context.FreeRegister(tempReg);
        }
    }

    void PostIncAndDecOp::Print(std::ostream &stream) const
    {
        expression_->Print(stream);
        stream << op_symbol_ << op_symbol_;
    }

    void PreIncAndDecOp::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string expr_name = expression_->GetID();
        expression_->EmitRISC(stream, context, destReg, type);
        if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE)
        {
            int tempReg = context.AssignRegister(type);
            int lc_n = context.AddFloatLiteralConstant(1.0, type);
            int srcMemReg = context.AssignRegister(TypeSpecifier::INT);
            stream << "lui " << context.GetRegString(srcMemReg) << ", %hi(.LC" << lc_n << ")" << std::endl;
            stream << GetLoadOp(type) << " " << context.GetRegString(tempReg) << ",%lo(.LC" << lc_n << ")(" << context.GetRegString(srcMemReg) << ")" << std::endl;
            stream << GetAddOrSubOp(type) << " " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << context.GetRegString(tempReg) << std::endl;
            context.FreeRegister(tempReg);
            if (context.VariableIsLocal(expr_name))
            {
                stream << GetStoreOp(type) << " " << context.GetRegString(destReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(expr_name).offset << "(s0)" << std::endl;
            }
            else
            {
                stream << "lui " << context.GetRegString(srcMemReg) << ",%hi(" << expr_name << ")" << std::endl;
                stream << GetStoreOp(type) << " " << context.GetRegString(destReg) << ",%lo(" << expr_name << ")(" << context.GetRegString(srcMemReg) << ")" << std::endl;
            }
            context.FreeRegister(srcMemReg);
        }
        else
        {
            stream << "addi " << context.GetRegString(destReg) << "," << context.GetRegString(destReg) << "," << addi_value_ << std::endl;
            if (context.VariableIsLocal(expr_name))
            {
                stream << "sw " << context.GetRegString(destReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(expr_name).offset << "(s0)" << std::endl;
            }
            else
            {
                int tempReg = context.AssignRegister(TypeSpecifier::INT);
                stream << "lui " << context.GetRegString(tempReg) << ",%hi(" << expr_name << ")" << std::endl;
                stream << "sw " << context.GetRegString(destReg) << ",%lo(" << expr_name << ")(" << context.GetRegString(tempReg) << ")" << std::endl;
                context.FreeRegister(tempReg);
            }
        }
    }

    void PreIncAndDecOp::Print(std::ostream &stream) const
    {
        stream << op_symbol_ << op_symbol_;
        expression_->Print(stream);
    }

} // namespace ast
