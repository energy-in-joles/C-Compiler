#pragma once

#include <string_view>
#include <stdexcept>
#include <unordered_map>

namespace ast
{
    // generalise loops to reference start and end labels for keywords (except IF)
    enum class ControlFlowType
    {
        FOR,
        WHILE,
        SWITCH,
    };

    enum class TypeSpecifier
    {
        INT,
        FLOAT,
        DOUBLE,
        UNSIGNED,
        CHAR,
        STRING,
        VOID
    };

    union DoubleUnion
    {
        double d;
        uint32_t reps[2];
    };
    union FloatUnion
    {
        float f;
        uint32_t rep;
    };

    int GetTypeSize(TypeSpecifier type);         // use for global and sizeof
    int GetTypeSizeForStack(TypeSpecifier type); // use for stack (keep it simple by assigning 4 bytes for char)

    std::string GetLoadOp(TypeSpecifier type);
    std::string GetStoreOp(TypeSpecifier type);
    std::string GetMoveOp(TypeSpecifier type);
    std::string GetArithmeticOp(TypeSpecifier type, std::string op);

    template <typename LogStream>
    LogStream &operator<<(LogStream &ls, const TypeSpecifier &type)
    {
        const auto TypeToString = [&type]
        {
            switch (type)
            {
            case TypeSpecifier::INT:
                return "int";
            case TypeSpecifier::FLOAT:
                return "float";
            case TypeSpecifier::DOUBLE:
                return "double";
            case TypeSpecifier::CHAR:
                return "char";
            case TypeSpecifier::VOID:
                return "void";
            case TypeSpecifier::STRING:
                return "string";
            case TypeSpecifier::UNSIGNED:
                return "unsigned";
            default:
                throw std::runtime_error("Unexpected type specifier");
            }
        };
        return ls << TypeToString();
    }

}
