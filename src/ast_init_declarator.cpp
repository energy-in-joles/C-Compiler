#include "ast_init_declarator.hpp"
#include "ast_direct_declarator.hpp"
#include <typeinfo>

namespace ast
{

    void InitDeclarator::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        // we can safely remove functions because they cannot be declared inside functions
        if (declarator_->IsFunction())
        {
            const DirectDeclarator &func_declarator = dynamic_cast<const DirectDeclarator &>(*declarator_);
            func_declarator.StoreFunctionInfo(type, context);
            return;
        }

        // update type to int if its a pointer
        type = declarator_->IsPointer(context, true) ? TypeSpecifier::INT : type;
        // handle local
        if (context.VariableIsLocal(declarator_->GetID()))
        {
            // assumes that declarator has already been declared
            if (initializer_)
            {
                int offset = context.GetCurrentFunctionContext().GetFunctionVariableInfo(declarator_->GetID()).offset;
                if (declarator_->IsArray())
                {
                    // handle char[] x = "hello"
                    if (initializer_->GetType(context) == TypeSpecifier::STRING)
                    {
                        int lc_n = context.AddStringLiteralConstant(initializer_->GetID());
                        int indexReg = context.AssignRegister(TypeSpecifier::INT);
                        stream << "lui " << context.GetRegString(indexReg) << ", %hi(.LC" << lc_n << ")" << std::endl;
                        stream << "addi " << context.GetRegString(indexReg) << ", " << context.GetRegString(indexReg) << ", %lo(.LC" << lc_n << ")" << std::endl;
                        for (int i = 0; i < initializer_->GetArraySize(context); i++)
                        {
                            stream << "lbu " << context.GetRegString(destReg) << ", " << i << "(" << context.GetRegString(indexReg) << ")" << std::endl;
                            EmitLocalDefinition(stream, context.GetRegString(destReg), offset, type);
                            offset += GetTypeSize(type); // shift by 1 byte not 4
                        }
                        context.FreeRegister(destReg);
                        context.FreeRegister(indexReg);
                    }
                    // handle regular array initialization
                    else
                    {
                        const NodeList &node_list = dynamic_cast<const NodeList &>(*initializer_);
                        for (const auto &node : node_list)
                        {
                            node->EmitRISC(stream, context, destReg, type);
                            EmitLocalDefinition(stream, context.GetRegString(destReg), offset, type);
                            offset += GetTypeSize(type);
                        }
                    }
                }
                else
                {
                    // we need to change the register if we are initializing a pointer
                    int srcReg = declarator_->IsPointer(context, true) ? context.AssignRegister(TypeSpecifier::INT) : destReg;
                    initializer_->EmitRISC(stream, context, srcReg, type);
                    EmitLocalDefinition(stream, context.GetRegString(srcReg), offset, type);
                    if (srcReg != destReg)
                    {
                        context.FreeRegister(srcReg);
                    }
                }
            }
            // do nothing for base case of int x where !initializer_ (we should not be emitting RISC when evaluating x)
        }
        // handle global
        else
        {
            stream << ".data" << std::endl;
            stream << ".globl " << declarator_->GetID() << std::endl;
            stream << declarator_->GetID() << ":" << std::endl;
            if (initializer_)
            {
                if (declarator_->IsArray())
                {
                    // handle array type string ie char x[] = "hello"
                    if (initializer_->GetType(context) == TypeSpecifier::STRING)
                    {
                        // TODO: check if need special case for "", if the array only contains null terminator
                        EmitGlobalDefinition(stream, std::any(0), TypeSpecifier::STRING, initializer_->GetID());
                    }
                    else
                    {
                        const NodeList &node_list = dynamic_cast<const NodeList &>(*initializer_);
                        for (const auto &node : node_list)
                        {
                            EmitGlobalDefinition(stream, node->GetValue(type), type, "");
                        }
                        // fill the rest with zeros
                        int undefined_size = GetTypeSize(type) * (declarator_->GetArraySize(context) - node_list.Size());
                        stream << ".zero " << undefined_size << std::endl;
                    }
                }
                else
                {
                    // handle char *x = "hello"
                    if (initializer_->GetType(context) == TypeSpecifier::STRING)
                    {
                        int lc_n = context.AddStringLiteralConstant(initializer_->GetID());
                        stream << ".word " << ".LC" << lc_n << std::endl;
                    }
                    else
                    {
                        EmitGlobalDefinition(stream, initializer_->GetValue(type), type, initializer_->GetID());
                    }
                }
            }
            else
            {
                int size = GetTypeSize(type);
                if (declarator_->IsArray())
                {
                    size *= declarator_->GetArraySize(context);
                }
                stream << ".zero " << size << std::endl;
            }
        }
    }

    void InitDeclarator::EmitLocalDefinition(std::ostream &stream, std::string srcRegStr, int offset, TypeSpecifier type) const
    {
        switch (type)
        {
        case TypeSpecifier::CHAR:
            stream << "sb " << srcRegStr << ", " << offset << "(s0)" << std::endl;
            break;
        case TypeSpecifier::INT:
            stream << "sw " << srcRegStr << ", " << offset << "(s0)" << std::endl;
            break;
        case TypeSpecifier::FLOAT:
            stream << "fsw " << srcRegStr << ", " << offset << "(s0)" << std::endl;
            break;
        case TypeSpecifier::DOUBLE:
            stream << "fsd " << srcRegStr << ", " << offset << "(s0)" << std::endl;
            break;
        default:
            std::runtime_error("InitDeclarator: Invalid Type.");
        }
    }

    // id is for handling cases for pointer to global variable
    void InitDeclarator::EmitGlobalDefinition(std::ostream &stream, std::any value, TypeSpecifier type, std::string id) const
    {
        if (type != TypeSpecifier::STRING && !id.empty())
        {
            stream << ".word " << id << std::endl;
        }
        else
        {
            switch (type)
            {
            case TypeSpecifier::CHAR:
                stream << ".byte" << std::any_cast<char>(value) << std::endl;
                break;
            case TypeSpecifier::INT:
                stream << ".word " << std::any_cast<int>(value) << std::endl;
                break;
            case TypeSpecifier::FLOAT:
            {
                union FloatUnion f_union = {.f = std::any_cast<float>(value)};
                stream << ".word " << f_union.rep << std::endl;
                break;
            }
            case TypeSpecifier::DOUBLE:
            {
                union DoubleUnion d_union = {.d = std::any_cast<double>(value)};
                stream << ".word " << d_union.reps[0] << std::endl;
                stream << ".word " << d_union.reps[1] << std::endl;
                break;
            }
            case TypeSpecifier::STRING:
                stream << ".string " << id << std::endl;
                break;
            default:
                std::runtime_error("InitDeclarator: Invalid Type.");
            }
        }
    }

    void InitDeclarator::Print(std::ostream &stream) const
    {
        declarator_->Print(stream);
        if (initializer_)
        {
            stream << " = ";
            initializer_->Print(stream);
        }
    }

    std::string InitDeclarator::GetID() const
    {
        return declarator_->GetID();
    }

    bool InitDeclarator::IsFunction() const
    {
        return declarator_->IsFunction();
    }

    bool InitDeclarator::IsPointer(Context &context, const bool has_been_declared) const
    {
        return declarator_->IsPointer(context, has_been_declared);
    }

    bool InitDeclarator::IsArray() const
    {
        return declarator_->IsArray();
    }

    int InitDeclarator::GetPointerDepth() const
    {
        return declarator_->GetPointerDepth();
    }

    int InitDeclarator::GetArraySize(Context &context) const
    {
        int size = declarator_->GetArraySize(context);

        // if empty square brackets
        if (size == -1)
        {
            return initializer_ ? initializer_->GetArraySize(context) : 1;
        }
        return size;
    }
}
