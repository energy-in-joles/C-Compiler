#pragma once

#include "ast_node.hpp"

namespace ast
{
    class IntConstant : public Node
    {
    private:
        int value_;
        std::string raw_str_;

    public:
        IntConstant(int value, std::string raw_str) : value_(value), raw_str_(raw_str) {}

        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

    class FloatConstant : public Node
    {
    private:
        double value_;
        std::string raw_str_;

    public:
        FloatConstant(double value, std::string raw_str) : value_(value), raw_str_(raw_str) {}
        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

    int EscapedCharMap(std::string s);

    class CharLiteral : public Node
    {
    private:
        std::string raw_str_;
        int value_;

        void Char2Int();

    public:
        CharLiteral(std::string raw_str) : raw_str_(raw_str) { Char2Int(); }
        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::any GetValue(TypeSpecifier type) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

    class StringLiteral : public Node
    {
    private:
        std::string raw_str_;

    public:
        StringLiteral(std::string raw_str) : raw_str_(raw_str) {}
        void EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const override;
        void Print(std::ostream &stream) const override;
        std::string GetID() const override;

        int GetArraySize(Context &context) const override;
        TypeSpecifier GetType(Context &context) const override;
    };

} // namespace ast
