#include "ast_enum.hpp"
#include <typeinfo>
#include <any>
#include "ast_constant.hpp"

namespace ast
{
    void EnumDeclaration::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)stream;
        (void)destReg;
        (void)type;

        if (!enumerators_)
        {
            return;
        }
        const NodeList *enum_list = dynamic_cast<const NodeList *>(enumerators_.get());
        if (!enum_list)
        {
            return;
        }

        int current_val = 0;
        for (const auto &enumerator : *enum_list)
        {
            const Enumerator *enum_node = dynamic_cast<const Enumerator *>(enumerator.get());
            if (!enum_node)
                continue;

            // First check if the value is explicitly defined
            if (enum_node->GetValue())
            {
                if (const auto *constant = dynamic_cast<const IntConstant *>(enum_node->GetValue().get()))
                {
                    current_val = std::any_cast<int>(constant->GetValue(TypeSpecifier::INT));
                }
            }

            // Add enum to global context
            context.AddEnum(enum_node->GetName(), current_val);
            current_val++;
        }
    }

    void EnumDeclaration::Print(std::ostream &stream) const
    {
        stream << "enum " << enum_name_ << " {" << std::endl;
        if (enumerators_ == nullptr)
        {
            stream << "};" << std::endl;
            return;
        }
        if (const NodeList *list = dynamic_cast<const NodeList *>(enumerators_.get()))
        {
            for (const auto &enumerator : *list)
            {
                const Enumerator *enum_node = dynamic_cast<const Enumerator *>(enumerator.get());
                if (!enum_node)
                    continue;

                stream << enum_node->GetName();
                if (enum_node->GetValue())
                {
                    stream << " = ";
                    enum_node->GetValue()->Print(stream);
                }
                stream << "," << std::endl;
            }
        }
        stream << "};" << std::endl;
    }

    void Enumerator::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        if (value_)
        {
            value_->EmitRISC(stream, context, destReg, type);
        }
        else if (context.InEnum(name_))
        {
            int value = context.GetEnumValue(name_);
            stream << "    li " << context.GetRegString(destReg) << ", " << value << " # Load enum value " << name_ << std::endl;
        }
        else
        {
            stream << "    # WARNING: Enum " << name_ << " not found" << std::endl;
            stream << "    li " << context.GetRegString(destReg) << ", 0 # Default value" << std::endl;
        }
    }

    void Enumerator::Print(std::ostream &stream) const
    {
        stream << name_;
        if (value_)
        {
            stream << " = ";
            value_->Print(stream);
        }
    }
}
