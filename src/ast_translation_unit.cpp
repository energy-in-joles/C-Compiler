#include "ast_translation_unit.hpp"
#include <stdexcept>

namespace ast
{
    void TranslationUnit::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        external_declarations_->EmitRISC(stream, context, destReg, type);
        std::vector<LiteralConstant> literal_constants = context.GetLiteralConstants();
        if (literal_constants.size() == 0)
        {
            return;
        }
        stream << ".section .rodata" << std::endl;
        for (size_t i = 0; i < literal_constants.size(); i++)
        {
            LiteralConstant literal_constant = literal_constants[i];
            stream << ".LC" << i << ":" << std::endl;
            switch (literal_constant.type)
            {
            case TypeSpecifier::FLOAT:
            {
                union FloatUnion f_union = {.f = static_cast<float>(literal_constant.value)};
                stream << ".align 2" << std::endl;
                stream << ".word " << f_union.rep << std::endl;
                break;
            }
            case TypeSpecifier::DOUBLE:
            {
                union DoubleUnion d_union = {.d = literal_constant.value};
                stream << ".align 3" << std::endl;
                stream << ".word " << d_union.reps[0] << std::endl;
                stream << ".word " << d_union.reps[1] << std::endl;
                break;
            }
            case TypeSpecifier::STRING:
            {
                stream << ".string " << literal_constant.str << std::endl;
                break;
            }
            default:
                throw std::runtime_error("TranslationUnit: Invalid Type.");
            }
        }
    }

    void TranslationUnit::Print(std::ostream &stream) const
    {
        external_declarations_->Print(stream);
    }
}
