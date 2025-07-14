#include "ast_context.hpp"
#include <stdexcept>
#include <iostream>
#include <fstream>

namespace ast
{
    const int N_REGISTERS = 64;
    const int START_SINGLE_REGISTER = 31; // start with a1, the previous registers are reserved
    const int START_FLOAT_REGISTER = 45;
    const int DEFAULT_STACK_SIZE = 32; // TODO: for now, just default as size 16, but should increse with more variables
    const int WORD_SIZE = 4;
    const int MAX_REGISTER_PARAM = 8;                 // after 8 params, we don't have registers to store them
    const std::string RESERVED_VARIABLE_PREFIX = "#"; // use this prefix for storing registers in the variable table

    // Register Definitions
    // x0               zero
    // x1               ra
    // x2               sp
    // x3               gp
    // x4               tp
    // x5 - x7          t0-t2
    // x8               s0/fp
    // x9               s1
    // x10 - x11        a0-a1
    // x12 - x17        a2-a7
    // x18 - x27        s2-s11
    // x28 - x31        t3-t6

    // Definitions for the static constants
    // DO NOT MODIFY
    // affects: function_definition.cpp
    const std::string registers[N_REGISTERS] = {
        "zero", "ra", "sp", "gp", "tp", "s0",
        "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
        "fs0", "fs1", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11",
        "fa0",
        "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
        "t0", "t1", "t2", "t3", "t4", "t5", "t6",
        "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7",
        "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7", "ft8", "ft9", "ft10", "ft11"};

    std::map<std::string, int> Context::CreateRegisterMap()
    {
        std::map<std::string, int> register_map;
        for (int i = 0; i < N_REGISTERS; i++)
        {
            register_map[registers[i]] = i;
        }
        return register_map;
    }

    std::set<int> Context::InitializeUnusedRegisters()
    {
        std::set<int> unused_registers;
        for (int i = START_SINGLE_REGISTER; i < N_REGISTERS; i++)
        {
            unused_registers.insert(i);
        }
        return unused_registers;
    }

    void Context::AddGlobalVariable(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth)
    {
        GlobalVariable var = {name, type, is_pointer, pointer_depth, false, 0};
        if (global_variable_table_.find(name) != global_variable_table_.end())
        {
            throw std::runtime_error("Global variable already declared");
        }
        global_variable_table_[name] = var;
    }

    void Context::AddFunctionInfo(const std::string &name, const std::vector<ParamInfo> &params, const TypeSpecifier returnType)
    {
        if (function_info_table_.find(name) != function_info_table_.end())
        {
            throw std::runtime_error("Function already declared");
        }
        FunctionInfo fun = {name, params, returnType};
        function_info_table_[name] = fun;
    }

    bool Context::VariableIsLocal(const std::string &name) const
    {
        // First check if it's an enum
        if (InEnum(name))
        {
            return true;
        }

        // Then check in current scope
        if (!function_context_stack_.empty())
        {
            FunctionContext fun = function_context_stack_.top();
            if (fun.VariableInLocalScope(name))
            {
                return true;
            }
        }
        if (global_variable_table_.find(name) != global_variable_table_.end())
        {
            return false;
        }
        else
        {
            throw std::runtime_error("Variable not found");
        }
    }

    bool Context::InGlobalScope() const
    {
        return function_context_stack_.empty();
    }

    FunctionInfo Context::GetFunctionInfo(const std::string &name) const
    {
        if (function_info_table_.find(name) != function_info_table_.end())
        {
            return function_info_table_.at(name);
        }
        else
        {
            throw std::runtime_error("Function not found");
        }
    }

    GlobalVariable Context::GetGlobalVariableInfo(const std::string &name) const
    {
        if (global_variable_table_.find(name) != global_variable_table_.end())
        {
            return global_variable_table_.at(name);
        }
        else
        {
            throw std::runtime_error("Global variable not found");
        }
    }

    int Context::AddFloatLiteralConstant(double value, TypeSpecifier type)
    {
        LiteralConstant lc = {value, "", type};
        literal_constants_.push_back(lc);
        return (literal_constants_.size() - 1);
    }

    int Context::AddStringLiteralConstant(std::string s)
    {
        LiteralConstant lc = {0, s, TypeSpecifier::STRING};
        literal_constants_.push_back(lc);
        return (literal_constants_.size() - 1);
    }

    std::vector<LiteralConstant> Context::GetLiteralConstants() const
    {
        return literal_constants_;
    }

    void Context::CreateNewFunctionScope(const std::string &funcID)
    {
        function_context_stack_.push(FunctionContext(funcID));
    }

    void Context::DestroyFunctionScope()
    {
        function_context_stack_.pop();
    }

    FunctionContext &Context::GetCurrentFunctionContext()
    {
        if (function_context_stack_.empty())
        {
            throw std::runtime_error("No function context found");
        }
        return function_context_stack_.top();
    }

    int Context::AssignRegister(TypeSpecifier type)
    {
        if (unused_registers_.empty())
        {
            throw std::out_of_range("Out of usable registers");
        }
        for (auto it = unused_registers_.begin(); it != unused_registers_.end(); ++it)
        {
            int reg = *it;
            if ((type == TypeSpecifier::INT || type == TypeSpecifier::CHAR || type == TypeSpecifier::VOID || type == TypeSpecifier::UNSIGNED) && reg < START_FLOAT_REGISTER)
            {
                unused_registers_.erase(reg);
                used_registers_.insert(reg);
                return reg;
            }
            else if ((type == TypeSpecifier::FLOAT || type == TypeSpecifier::DOUBLE) && reg >= START_FLOAT_REGISTER)
            {
                unused_registers_.erase(reg);
                used_registers_.insert(reg);
                return reg;
            }
        }
        throw std::out_of_range("Out of usable registers or register type mismatch");
    }

    void Context::UseRegister(const std::string reg_name)
    {
        int reg = register_map_.at(reg_name);
        if (used_registers_.find(reg) != used_registers_.end())
        {
            throw std::runtime_error("Register " + reg_name + " already in use");
        }
        used_registers_.insert(reg);
        unused_registers_.erase(reg);
    }

    void Context::FreeRegister(const int reg)
    {
        if (reg >= START_SINGLE_REGISTER && reg < N_REGISTERS)
        {
            unused_registers_.insert(reg);
            used_registers_.erase(reg);
        }
        else
        {
            throw std::runtime_error("Free Register: register i" + std::to_string(reg) + " not found or not usable");
        }
    }

    void Context::FreeRegister(const std::string reg_name)
    {
        int reg = register_map_.at(reg_name);
        FreeRegister(reg);
    }

    const std::set<int> Context::GetUsedRegisters() const
    {
        return used_registers_;
    }

    std::string Context::GenerateUniqueLabel(const std::string &labelID)
    {
        static int label_counter_ = 0;
        return "." + labelID + std::to_string(label_counter_++);
        // TODO: Test with multiple ifs in a function
    }

    std::string Context::GetRegString(const int reg) const
    {
        if (reg < 0 || reg >= N_REGISTERS)
        {
            throw std::runtime_error("Register " + std::to_string(reg) + " not found");
        }
        return registers[reg];
    }

    int Context::GetRegIndex(const std::string &reg_name) const
    {
        if (register_map_.find(reg_name) == register_map_.end())
        {
            throw std::runtime_error("Register " + reg_name + " not found");
        }
        return register_map_.at(reg_name);
    }

    void Context::NewLoopContext(ControlFlowType type, const std::string &start_label, const std::string &end_label, const std::string &update_label)
    {
        loop_context_stack_.push({type, start_label, end_label, update_label});
    }

    void Context::EndLoopContext()
    {
        if (!loop_context_stack_.empty())
        {
            loop_context_stack_.pop();
        }
    }

    LoopContext Context::GetCurrentLoopContext() const
    {
        if (!loop_context_stack_.empty())
        {
            return loop_context_stack_.top();
        }
        throw std::runtime_error("No loop context found");
    }

    std::string Context::GetStartLabel() const
    {
        return GetCurrentLoopContext().start_label;
    }

    std::string Context::GetEndLabel() const
    {
        return GetCurrentLoopContext().end_label;
    }

    std::string Context::GetUpdateLabel() const
    {
        return GetCurrentLoopContext().update_label;
    }

    void Context::AddEnum(const std::string &name, const int value)
    {
        enum_table_[name] = value;
        std::cerr << "Added enum: " << name << " = " << value << std::endl;
    }

    bool Context::InEnum(const std::string &name) const
    {
        return enum_table_.find(name) != enum_table_.end();
    }

    int Context::GetEnumValue(const std::string &name) const
    {
        auto it = enum_table_.find(name);
        if (it != enum_table_.end())
        {
            return it->second;
        }
        // Handle error case - enum not found
        // This should never happen if you check with InEnum first
        return 0;
    }

    void Context::AddGlobalArray(const std::string &name, int size, TypeSpecifier type)
    {
        if (global_variable_table_.find(name) != global_variable_table_.end())
        {
            throw std::runtime_error("Global array already declared");
        }
        GlobalVariable array = {name, type, false, 0, true, size};
        global_variable_table_[name] = array;
    }

    int Context::GetPointerOffset(const std::string &name, int my_pointer_depth)
    {
        // TODO: add pointer to struct lmaooo
        int pointer_depth;
        TypeSpecifier type;
        if (VariableIsLocal(name))
        {
            pointer_depth = GetCurrentFunctionContext().GetFunctionVariableInfo(name).pointer_depth;
            type = GetCurrentFunctionContext().GetFunctionVariableInfo(name).type;
        }
        else
        {
            pointer_depth = GetGlobalVariableInfo(name).pointer_depth;
            type = GetGlobalVariableInfo(name).type;
        }
        // if child is a pointer to a pointer, we default to shifting by 4
        if (my_pointer_depth + 1 < pointer_depth)
        {
            return GetTypeSize(TypeSpecifier::INT);
        }
        return GetTypeSize(type);
    }

    std::ostringstream Context::PrintContext() const
    {
        std::ostringstream buffer;
        buffer << "Global Variables: " << std::endl;
        for (auto const &var : global_variable_table_)
        {
            buffer << var.first << " : " << var.second.type << std::endl;
        }
        buffer << "Unused Registers: " << std::endl;
        for (auto const &reg : unused_registers_)
        {
            buffer << reg << ", ";
        }
        buffer << std::endl
               << std::endl;

        buffer << "Literal Constants: " << std::endl;
        for (auto const &lc : literal_constants_)
        {
            buffer << lc.value << " : " << lc.type << std::endl;
        }
        buffer << std::endl
               << std::endl;

        if (!function_context_stack_.empty())
        {
            function_context_stack_.top().PrintFunctionContext(buffer);
        }
        return buffer;
    }

    // ----------  FUNCTION CONTEXT  ------------------
    std::string FunctionContext::GetName() const
    {
        return name_;
    }

    std::string FunctionContext::GetEndLabel() const
    {
        return "." + name_ + "_func_end";
    }

    void FunctionContext::EnterScope()
    {
        Scope new_frame;
        new_frame.scope_pointer_ = stack_pointer_offset_; // save pointer location
        variable_table_stack_.push(new_frame);            // push new frame to stack (similar to protected access)
    }

    void FunctionContext::ExitScope()
    {
        if (variable_table_stack_.size() <= 1)
        {
            throw std::runtime_error("Cannot exit the outermost scope");
        }
        Scope last_frame = variable_table_stack_.top(); // get the top frame (current scope/inner most scope)
        variable_table_stack_.pop();
        stack_pointer_offset_ = last_frame.scope_pointer_;
    }

    void FunctionContext::AddFunctionVariable(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth)
    {
        Scope &current_frame = variable_table_stack_.top();
        auto &variables = current_frame.variable_table_;
        // check if variable already exists in the table
        if (variables.find(name) != variables.end())
        {
            if (name[0] == RESERVED_VARIABLE_PREFIX[0])
            {
                return;
            }
            throw std::runtime_error("Variable " + name + " already declared in function context");
        }

        if (!is_pointer)
        {
            stack_pointer_offset_ -= GetTypeSizeForStack(type);
        }
        else
        {
            stack_pointer_offset_ -= GetTypeSizeForStack(TypeSpecifier::INT);
        }

        if (stack_pointer_offset_ + stack_size_ <= 0)
        {
            stack_size_ *= 2; // double stack size when it's full
        }

        // Initialize ALL struct fields
        FunctionVariable var = {name, type, is_pointer, pointer_depth, stack_pointer_offset_, false, 0};
        variables[name] = var;
    }

    void FunctionContext::AddArray(const std::string &name, int size, TypeSpecifier type)
    {
        Scope &current_scope = variable_table_stack_.top();
        int element_size = GetTypeSize(type);
        int total_size = size * element_size;

        // Align stack offset
        stack_pointer_offset_ -= total_size;
        int off = WORD_SIZE - (total_size % WORD_SIZE);
        stack_pointer_offset_ -= off;

        current_scope.variable_table_[name] = {
            .name = name,
            .type = type,
            .is_pointer = false,
            .pointer_depth = 0,
            .offset = stack_pointer_offset_,
            .is_array = true,
            .array_size = size};

        // use while loop because array can grow much larger than stack size
        while (stack_pointer_offset_ + stack_size_ <= 0)
        {
            stack_size_ *= 2; // double stack size when it's full
        }
        FunctionVariable var = {name, type, false, 0, stack_pointer_offset_, true, size};
        current_scope.variable_table_[name] = var;
    }

    void FunctionContext::AllocateStackSpaceForParams(const std::vector<ParamInfo> param_infos)
    {
        int n_single_params = 0;
        int n_float_params = 0;
        int sp_offset = 0;
        for (const ParamInfo &param_info : param_infos)
        {
            TypeSpecifier type = param_info.type;
            type = param_info.is_pointer ? TypeSpecifier::INT : type; // if it's a pointer, treat it as an int
            switch (type)
            {
            case TypeSpecifier::INT:
                if (n_single_params > MAX_REGISTER_PARAM)
                {
                    sp_offset += GetTypeSizeForStack(type);
                }
                else
                {
                    n_single_params++;
                }
                break;
            case TypeSpecifier::FLOAT:
                n_float_params++;
                if (n_float_params > MAX_REGISTER_PARAM)
                {
                    if (n_single_params < MAX_REGISTER_PARAM)
                    {
                        n_single_params++;
                    }
                    sp_offset += GetTypeSizeForStack(type); // we have to move this offset even on 'a' register pass
                    // because we store float in memory before moving to 'a' registers
                }
                else
                {
                    n_float_params++;
                }
                break;
            case TypeSpecifier::DOUBLE:
                n_float_params++;
                if (n_float_params > MAX_REGISTER_PARAM)
                {
                    if (n_single_params < MAX_REGISTER_PARAM - 1)
                    {
                        n_single_params += 2;
                    }
                    // worst case where a double is split between an 'a' register and memory
                    else if (n_single_params < MAX_REGISTER_PARAM)
                    {
                        n_single_params++;
                    }
                    sp_offset += GetTypeSizeForStack(type);
                }
                else
                {
                    n_float_params++;
                }
                break;
            default:
                throw std::runtime_error("Invalid type in AllocateStackSpaceForParams");
            }

            while (sp_offset - stack_size_ >= stack_pointer_offset_)
            {
                stack_size_ *= 2; // double stack size when it's full
            }
        }
    }

    void FunctionContext::AcceptParamFromStack(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth, const int offset)
    {
        Scope &current_frame = variable_table_stack_.top();
        // TODO accept array params; for now, assume no array params
        FunctionVariable var = {name, type, is_pointer, pointer_depth, offset, false, 0};
        current_frame.variable_table_[name] = var;
    }

    bool FunctionContext::VariableInLocalScope(const std::string &name) const
    {
        std::stack<Scope> temp_stack = variable_table_stack_;
        while (!temp_stack.empty())
        {
            if (temp_stack.top().variable_table_.find(name) !=
                temp_stack.top().variable_table_.end())
            {
                return true;
            }
            temp_stack.pop();
        }
        return false;
    }

    FunctionVariable FunctionContext::GetFunctionVariableInfo(const std::string &name) const
    {
        std::stack<Scope> temp_stack = variable_table_stack_;
        while (!temp_stack.empty())
        {
            const auto &frame = temp_stack.top();
            if (frame.variable_table_.count(name))
            {
                return frame.variable_table_.at(name);
            }
            temp_stack.pop();
        }
        return FunctionVariable();
    }

    int FunctionContext::GetStackSize() const
    {
        return stack_size_;
    }

    void FunctionContext::PrintFunctionContext(std::ostream &stream) const
    {
        stream << "FunctionContext: " << std::endl;
        stream << "Name: " << name_ << std::endl;
        stream << "Stack Pointer Offset: " << stack_pointer_offset_ << std::endl;
        stream << "Stack Size: " << stack_size_ << std::endl;
        stream << "Variable Table {" << std::endl;

        std::stack<Scope> temp_stack = variable_table_stack_;
        std::vector<Scope> frames;

        while (!temp_stack.empty())
        {
            frames.push_back(temp_stack.top());
            temp_stack.pop();
        }

        std::reverse(frames.begin(), frames.end()); // set outer scope to be at the top of the stack

        for (auto const &frame : frames)
        {
            for (auto const &pair : frame.variable_table_)
            {
                stream << pair.first << " : " << pair.second.type << " : " << pair.second.offset << std::endl;
            }
        }
        stream << "}" << std::endl;
    }

} // namespace ast
