#include "ast_direct_declarator.hpp"
#include "ast_function_call.hpp"

namespace ast
{

    // ONLY CALLED DURING FUNCTION DEFINITION
    void DirectDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)destReg;
        (void)type;
        // TODO: double check if we need to store registers here or free them`
        if (parameter_list_ != nullptr)
        {
            int single_reg_index = 0;
            int float_reg_index = 0;
            int stack_offset = 0;
            std::vector<ParamInfo> param_infos = parameter_list_->GetParams(context);
            for (auto &param : param_infos)
            {
                std::vector<std::string> registers_passed;
                TypeSpecifier param_type = param.type;
                param_type = param.is_pointer ? TypeSpecifier::INT : param_type; // if it's a pointer, treat it as an int

                // if we can retrieve from 'fa' registers
                if ((param_type == TypeSpecifier::FLOAT || param_type == TypeSpecifier::DOUBLE) && float_reg_index < MAX_REGISTER_PARAM)
                {
                    std::string register_passed = "fa" + std::to_string(float_reg_index);
                    registers_passed.push_back(register_passed);
                    float_reg_index++;
                }

                // if we can retrieve from 'a' registers
                if (registers_passed.empty() && single_reg_index < MAX_REGISTER_PARAM)
                {
                    int count = 1;
                    // for double case, check that there are two 'a' registers available
                    if (param_type == TypeSpecifier::DOUBLE && single_reg_index < MAX_REGISTER_PARAM - 1)
                    {
                        count += 1;
                    }
                    for (int i = 0; i < count; i++)
                    {
                        std::string register_passed = "a" + std::to_string(single_reg_index);
                        registers_passed.push_back(register_passed);
                        single_reg_index++;
                    }
                }

                if (!registers_passed.empty())
                {
                    // literally the worst case, where we pass a double to an 'a' register and memory
                    // we store in on the stack to span across -4(s0) to 3(s0).
                    if (param_type == TypeSpecifier::DOUBLE && registers_passed.size() == 1 && registers_passed[0][0] == 'a')
                    {
                        stream << "sw " << registers_passed[0] << ",-4(s0)" << std::endl; // we saved a space for this worst case double on every stack frame
                        stack_offset += WORD_SIZE;
                        context.GetCurrentFunctionContext().AcceptParamFromStack(param.name, param.type, param.is_pointer, param.pointer_depth, -4);
                    }
                    // normal case where we fully pass into registers
                    else
                    {
                        context.GetCurrentFunctionContext().AddFunctionVariable(param.name, param.type, param.is_pointer, param.pointer_depth);
                        int i = 0;
                        for (auto &register_passed : registers_passed)
                        {
                            // need to enforce this because we might be using an 'a' register to pass a float/double
                            if (register_passed[0] == 'a')
                            {
                                stream << "sw " << register_passed << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(param.name).offset + i * WORD_SIZE << "(s0)" << std::endl;
                            }
                            else
                            {
                                stream << GetStoreOp(param_type) << " " << register_passed << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(param.name).offset + i * WORD_SIZE << "(s0) " << std::endl;
                            }
                        }
                    }
                }
                else
                {
                    context.GetCurrentFunctionContext().AcceptParamFromStack(param.name, param.type, param.is_pointer, param.pointer_depth, stack_offset);
                    stack_offset += GetTypeSizeForStack(param.type);
                }
            }
        }
    }

    void DirectDeclarator::StoreFunctionInfo(TypeSpecifier return_type, Context &context) const
    {
        std::vector<ParamInfo> param_infos;
        if (parameter_list_ != nullptr)
        {
            param_infos = parameter_list_->GetParams(context);
        }

        context.AddFunctionInfo(identifier_->GetID(), param_infos, return_type);
    }

    void DirectDeclarator::Print(std::ostream &stream) const
    {
        identifier_->Print(stream);
        if (parameter_list_ != nullptr)
        {
            stream << "(";
            parameter_list_->Print(stream);
            stream << ")";
        }
    }

    std::string DirectDeclarator::GetID() const
    {
        return identifier_->GetID();
    }

} // namespace ast
