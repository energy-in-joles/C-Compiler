#pragma once
#include <sstream>
#include <map>
#include <string>
#include <unordered_map>
#include <set>
#include <stack>
#include <vector>
#include "ast_type_specifier.hpp"

namespace ast
{
    extern const int DEFAULT_STACK_SIZE;
    extern const int WORD_SIZE;
    extern const int MAX_REGISTER_PARAM;
    extern const std::string RESERVED_VARIABLE_PREFIX;

    struct FunctionVariable
    {
        std::string name;
        TypeSpecifier type;
        bool is_pointer;
        int pointer_depth;
        int offset;
        bool is_array;
        int array_size;
    };

    struct GlobalVariable
    {
        std::string name;
        TypeSpecifier type;
        bool is_pointer;
        int pointer_depth;
        bool is_array;
        int array_size;
    };

    struct ParamInfo
    {
        std::string name;
        TypeSpecifier type;
        bool is_pointer;
        int pointer_depth;
    };

    struct FunctionInfo
    {
        std::string name;
        std::vector<ParamInfo> params;
        TypeSpecifier return_type;
    };

    struct LiteralConstant
    {
        double value;
        std::string str;
        TypeSpecifier type;
    };

    struct LoopContext
    {
        ControlFlowType type;

        std::string start_label;
        std::string end_label;
        std::string update_label; // for loops only
    };

    class FunctionContext
    {
    private:
        // using a struct to make it easy to replicate for each scope
        struct Scope
        {
            int scope_pointer_;
            std::unordered_map<std::string, FunctionVariable> variable_table_;
        };
        const std::string name_;                 // Identifier of current function
        std::stack<Scope> variable_table_stack_; // stack for each scope
        int stack_pointer_offset_;
        int stack_size_;

    public:
        // initially offset stack by 8 because ra and s0 are stored in the first word and second word
        FunctionContext(std::string name)
            : name_(name), stack_pointer_offset_(-WORD_SIZE * 3), stack_size_(DEFAULT_STACK_SIZE)
        {
            Scope initial_frame;
            initial_frame.scope_pointer_ = stack_pointer_offset_;
            variable_table_stack_.push(initial_frame);
        };

        // deal with variables in function scope
        std::string GetName() const;
        std::string GetEndLabel() const;

        // ---- scope management -----
        void EnterScope();
        void ExitScope();

        void AddFunctionVariable(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth = 0);
        void AllocateStackSpaceForParams(const std::vector<ParamInfo>);
        void AcceptParamFromStack(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth, const int offset);

        bool VariableInLocalScope(const std::string &name) const;                // verify that this variable name is defined in local scope
        FunctionVariable GetFunctionVariableInfo(const std::string &name) const; // get function info including stack offset for function variable
        int GetStackSize() const;

        // ---- array management ----
        void AddArray(const std::string &name, int size, TypeSpecifier type);

        void PrintFunctionContext(std::ostream &stream) const; // for debugging

    }; // namespace ast

    class Context
    {
    private:
        const std::map<std::string, int> register_map_; // Static map declaration
        std::set<int> unused_registers_;                // Set of unused registers
        std::set<int> used_registers_;                  // Set of used registers
        std::map<std::string, int> CreateRegisterMap(); // build register_map_ and unused_registers_ at the start
        std::set<int> InitializeUnusedRegisters();

        std::unordered_map<std::string, GlobalVariable> global_variable_table_; // Set of declared global variables
        std::unordered_map<std::string, FunctionInfo> function_info_table_;     // Set of declared functions and their types
        std::stack<FunctionContext> function_context_stack_;                    // Stack of function contexts
        std::vector<LiteralConstant> literal_constants_;

        std::stack<LoopContext> loop_context_stack_;

        std::unordered_map<std::string, int> enum_table_;

    public:
        Context() : register_map_(CreateRegisterMap()), unused_registers_(InitializeUnusedRegisters()) {}

        // ----- dealing with variables in global scope ------
        void AddGlobalVariable(const std::string &name, const TypeSpecifier type, const bool is_pointer, const int pointer_depth = 0);
        void AddFunctionInfo(const std::string &name, const std::vector<ParamInfo> &params, const TypeSpecifier returnType);
        bool VariableIsLocal(const std::string &name) const;
        bool InGlobalScope() const;
        FunctionInfo GetFunctionInfo(const std::string &name) const;
        GlobalVariable GetGlobalVariableInfo(const std::string &name) const;

        // ---- dealing with literal constants like float and double ----
        int AddFloatLiteralConstant(double value, TypeSpecifier type);
        int AddStringLiteralConstant(std::string s);
        std::vector<LiteralConstant> GetLiteralConstants() const;

        // ---- function related context -----
        void CreateNewFunctionScope(const std::string &funcID);
        void DestroyFunctionScope();
        FunctionContext &GetCurrentFunctionContext();

        // ---- register management -----
        int AssignRegister(TypeSpecifier type);
        void UseRegister(std::string reg_name);
        void FreeRegister(int reg);
        void FreeRegister(std::string reg_name);
        const std::set<int> GetUsedRegisters() const; // get used registers
        std::string GetRegString(int reg) const;
        int GetRegIndex(const std::string &reg_name) const;
        std::string GenerateUniqueLabel(const std::string &labelID);

        // ---- loop context management ----
        void NewLoopContext(ControlFlowType type,
                            const std::string &start_label,   // Always required except if
                            const std::string &end_label,     // Always required
                            const std::string &update_label); // for loops only
        void EndLoopContext();
        LoopContext GetCurrentLoopContext() const;
        std::string GetStartLabel() const;
        std::string GetEndLabel() const;
        std::string GetUpdateLabel() const;

        // ---- enum context management ----
        void AddEnum(const std::string &name, int value);
        int GetEnumValue(const std::string &name) const;
        bool InEnum(const std::string &name) const;

        // ---- array context management ----
        void AddGlobalArray(const std::string &name, int size, TypeSpecifier type);

        int GetPointerOffset(const std::string &name, int pointer_depth);

        std::ostringstream PrintContext() const; // for debugging
    };
}
