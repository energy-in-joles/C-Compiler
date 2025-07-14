#include "ast_type_specifier.hpp"

namespace ast
{
    const std::unordered_map<TypeSpecifier, std::string> storeOp = {
        {TypeSpecifier::INT, "sw"},
        {TypeSpecifier::CHAR, "sb"},
        {TypeSpecifier::UNSIGNED, "sw"},
        {TypeSpecifier::FLOAT, "fsw"},
        {TypeSpecifier::DOUBLE, "fsd"},
    };

    const std::unordered_map<TypeSpecifier, std::string> loadOp = {
        {TypeSpecifier::INT, "lw"},
        {TypeSpecifier::UNSIGNED, "lw"},
        {TypeSpecifier::CHAR, "lbu"},
        {TypeSpecifier::FLOAT, "flw"},
        {TypeSpecifier::DOUBLE, "fld"},
    };

    const std::unordered_map<TypeSpecifier, int> typeSize = {
        {TypeSpecifier::INT, 4},
        {TypeSpecifier::UNSIGNED, 4},
        {TypeSpecifier::FLOAT, 4},
        {TypeSpecifier::DOUBLE, 8},
        {TypeSpecifier::CHAR, 1},
        {TypeSpecifier::VOID, 0},
    };

    const std::unordered_map<TypeSpecifier, int> typeSizeForStack = {
        {TypeSpecifier::INT, 4},
        {TypeSpecifier::UNSIGNED, 4},
        {TypeSpecifier::FLOAT, 4},
        {TypeSpecifier::DOUBLE, 8},
        {TypeSpecifier::CHAR, 4},
        {TypeSpecifier::VOID, 0},
    };

    const std::unordered_map<TypeSpecifier, std::string> moveOp = {
        {TypeSpecifier::INT, "mv"},
        {TypeSpecifier::UNSIGNED, "mv"},
        {TypeSpecifier::CHAR, "mv"},
        {TypeSpecifier::FLOAT, "fmv.s"},
        {TypeSpecifier::DOUBLE, "fmv.d"},
    };

    int GetTypeSize(TypeSpecifier type)
    {
        if (typeSize.find(type) == typeSize.end())
        {
            throw std::runtime_error("Invalid type specifier for size");
        }
        return typeSize.at(type);
    }

    int GetTypeSizeForStack(TypeSpecifier type)
    {
        if (typeSizeForStack.find(type) == typeSizeForStack.end())
        {
            throw std::runtime_error("Invalid type specifier for size");
        }
        return typeSizeForStack.at(type);
    }

    std::string GetLoadOp(TypeSpecifier type)
    {
        if (loadOp.find(type) == loadOp.end())
        {
            throw std::runtime_error("Invalid type specifier for load operation");
        }
        return loadOp.at(type);
    }

    std::string GetStoreOp(TypeSpecifier type)
    {
        if (storeOp.find(type) == storeOp.end())
        {
            throw std::runtime_error("Invalid type specifier for store operation");
        }
        return storeOp.at(type);
    }

    std::string GetMoveOp(TypeSpecifier type)
    {
        if (moveOp.find(type) == moveOp.end())
        {
            throw std::runtime_error("Invalid type specifier for move operation");
        }
        return moveOp.at(type);
    }

    std::string GetArithmeticOp(TypeSpecifier type, std::string op)
    {
        if (type == TypeSpecifier::INT || type == TypeSpecifier::CHAR)
        {
            return op;
        }
        else if (type == TypeSpecifier::FLOAT)
        {
            return "f" + op + ".s";
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            return "f" + op + ".d";
        }
        else
        {
            throw std::runtime_error("Invalid type specifier for arithmetic operation");
        }
    }

}
