#include "ast_function_call.hpp"

namespace ast
{
    std::vector<std::string> SaveUsedRegisters(Context &context, std::ostream &stream)
    {
        std::vector<std::string> saved_registers;
        std::set<int> used_registers_copy = context.GetUsedRegisters();
        for (const int &reg : used_registers_copy)
        {
            std::string register_name = context.GetRegString(reg);
            saved_registers.push_back(register_name);
            std::string reserved_name = RESERVED_VARIABLE_PREFIX + register_name;
            // TODO: we need to dynamically change the stack stored based on type
            TypeSpecifier type;
            if (register_name[0] == 'f')
            {
                // handle worst case scenario where we have to store a double
                type = TypeSpecifier::DOUBLE;
            }
            else
            {
                type = TypeSpecifier::INT;
            }
            context.GetCurrentFunctionContext().AddFunctionVariable(reserved_name, type, false);
            stream << GetStoreOp(type) << " " << context.GetRegString(reg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(reserved_name).offset << "(s0)" << std::endl;
            context.FreeRegister(reg);
        }
        return saved_registers;
    }

    void RestoreUsedRegisters(Context &context, std::ostream &stream, const std::vector<std::string> &saved_registers)
    {
        for (const std::string &register_name : saved_registers)
        {
            std::string reserved_name = RESERVED_VARIABLE_PREFIX + register_name;
            TypeSpecifier type;
            if (register_name[0] == 'f')
            {
                type = TypeSpecifier::DOUBLE;
            }
            else
            {
                type = TypeSpecifier::INT;
            }
            stream << GetLoadOp(type) << " " << register_name << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(reserved_name).offset << "(s0)" << std::endl;
            context.UseRegister(register_name);
        }
    }

    void FunctionCall::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        std::string function_name = postfix_expression_->GetID();
        FunctionContext &function_context = context.GetCurrentFunctionContext();
        std::vector<std::string> used_register_vec;
        std::vector<std::string> param_register_vec;
        FunctionInfo function_info = context.GetFunctionInfo(function_name);
        if (argument_expression_list_ != nullptr)
        {
            // save used registers
            used_register_vec = SaveUsedRegisters(context, stream);
            const NodeList &node_list = dynamic_cast<const NodeList &>(*argument_expression_list_);
            function_context.AllocateStackSpaceForParams(function_info.params);

            int single_register_i = 0;
            int float_register_i = 0;
            int node_i = 0;
            int stack_offset = 0;
            for (const auto &node : node_list)
            {
                std::vector<std::string> registers_to_save;
                TypeSpecifier param_type = function_info.params[node_i].type;
                bool is_pointer = function_info.params[node_i].is_pointer;
                param_type = is_pointer ? TypeSpecifier::INT : param_type;

                // check if we can pass float/double in 'fa' registers
                if ((param_type == TypeSpecifier::FLOAT || param_type == TypeSpecifier::DOUBLE) && float_register_i < MAX_REGISTER_PARAM)
                {
                    std::string register_to_save = "fa" + std::to_string(float_register_i);
                    registers_to_save.push_back(register_to_save);
                    float_register_i++;
                }

                // check if we can pass float/double/int/char in registers
                if (registers_to_save.empty() && single_register_i < MAX_REGISTER_PARAM)
                {
                    int count = 1;
                    if (param_type == TypeSpecifier::DOUBLE && single_register_i < MAX_REGISTER_PARAM - 1)
                    {
                        count += 1;
                    }
                    for (int i = 0; i < count; i++)
                    {
                        std::string register_to_save = "a" + std::to_string(single_register_i);
                        registers_to_save.push_back(register_to_save);
                        single_register_i++;
                    }
                }

                if (!registers_to_save.empty())
                {
                    for (auto &register_to_save : registers_to_save)
                    {
                        if (register_to_save[register_to_save.size() - 1] != '0')
                        {
                            context.UseRegister(register_to_save); // never add a0 into used registers list
                            param_register_vec.push_back(register_to_save);
                        }
                    }
                    // handle case where we are passing a float/double in 'a' registers
                    // we have to store the float/double in memory and load it back into the 'a' registers. What a pain!
                    if (registers_to_save[0][0] == 'a' && (param_type == TypeSpecifier::FLOAT || param_type == TypeSpecifier::DOUBLE))
                    {
                        int tmpReg = context.AssignRegister(param_type);
                        node->EmitRISC(stream, context, tmpReg, param_type);
                        std::string tmp_name = RESERVED_VARIABLE_PREFIX + std::to_string(node_i);
                        context.GetCurrentFunctionContext().AddFunctionVariable(tmp_name, param_type, false); // we can do this now, because we allocated space for all non 'fa' register floats/doubles
                        stream << GetStoreOp(param_type) << " " << context.GetRegString(tmpReg) << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(tmp_name).offset << "(s0)" << std::endl;

                        // literally the worst possible case where a double is SPLIT between an 'a' register and memory
                        if (param_type == TypeSpecifier::DOUBLE && registers_to_save.size() == 1)
                        {
                            if (stack_offset != 0)
                            {
                                std::runtime_error("FunctionCall: Stack offset is not 0 when passing split double argument.");
                            }
                            int tmpReg2 = context.AssignRegister(TypeSpecifier::INT);
                            stream << "lw " << context.GetRegString(tmpReg2) << "," << (context.GetCurrentFunctionContext().GetFunctionVariableInfo(tmp_name).offset + WORD_SIZE) << "(s0)" << std::endl;
                            stream << "sw " << context.GetRegString(tmpReg2) << "," << "0(sp)" << std::endl;
                            stack_offset += WORD_SIZE;
                            context.FreeRegister(tmpReg2);
                        }
                        context.FreeRegister(tmpReg);
                        int i = 0;
                        for (const std::string &register_to_save : registers_to_save)
                        {
                            stream << "lw " << register_to_save << "," << context.GetCurrentFunctionContext().GetFunctionVariableInfo(tmp_name).offset + i * WORD_SIZE << "(s0)" << std::endl;
                        }
                    }
                    // normal case where we are passing an int/char in 'a' registers or a float/double in 'fa' registers
                    else
                    {
                        node->EmitRISC(stream, context, context.GetRegIndex(registers_to_save[0]), param_type);
                    }
                }
                // store the rest of the parameters in memory
                else
                {
                    int tempReg = context.AssignRegister(param_type);
                    node->EmitRISC(stream, context, tempReg, param_type);
                    stream << GetStoreOp(param_type) << " " << context.GetRegString(tempReg) << "," << stack_offset << "(sp)" << std::endl;
                    context.FreeRegister(tempReg);
                    stack_offset += GetTypeSizeForStack(param_type);
                }
                node_i++;
            }
        }
        stream << "call " << function_name << std::endl;

        // restore all registers to previous state;
        for (const std::string &param_register : param_register_vec)
        {
            context.FreeRegister(param_register);
        }
        RestoreUsedRegisters(context, stream, used_register_vec);

        // ensure that the return type matches the expected return type
        if (function_info.return_type != type)
        {
            throw std::runtime_error("FunctionCall: Return type does not match.");
        }

        // store return value
        if (type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE)
        {
            stream << GetMoveOp(type) << " " << context.GetRegString(destReg) << ", fa0" << std::endl;
        }
        else
        {
            stream << "mv " << context.GetRegString(destReg) << ", a0" << std::endl;
        }
    }

    void FunctionCall::Print(std::ostream &stream) const
    {
        postfix_expression_->Print(stream);
        stream << "(";
        if (argument_expression_list_ != nullptr)
        {
            argument_expression_list_->Print(stream);
        }
        stream << ")";
    }

    TypeSpecifier FunctionCall::GetType(Context &context) const
    {
        return context.GetFunctionInfo(postfix_expression_->GetID()).return_type;
    }

} // namespace ast
