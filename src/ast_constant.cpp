#include "ast_constant.hpp"
#include <bitset>
namespace ast
{
    std::any IntConstant::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::INT)
        {
            return value_;
        }
        else
        {
            throw std::runtime_error("IntConstant::GetValue called with non-int type");
        }
    }

    void IntConstant::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        if (type != TypeSpecifier::INT && type != TypeSpecifier::CHAR)
        {
            throw std::runtime_error("IntConstant::EmitRISC called with non-int type");
        }
        stream << "li " << context.GetRegString(destReg) << ", " << value_ << std::endl;
    }

    TypeSpecifier IntConstant::GetType(Context &context) const
    {
        (void)context;
        return TypeSpecifier::INT;
    }

    void IntConstant::Print(std::ostream &stream) const
    {
        stream << raw_str_;
    }

    void FloatConstant::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        int lc_n = context.AddFloatLiteralConstant(value_, type);
        int srcReg = context.AssignRegister(TypeSpecifier::INT);
        stream << "lui " << context.GetRegString(srcReg) << ", %hi(.LC" << lc_n << ")" << std::endl;
        if (type == TypeSpecifier::FLOAT)
        {
            stream << "flw " << context.GetRegString(destReg) << ",%lo(.LC" << lc_n << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            stream << "fld " << context.GetRegString(destReg) << ",%lo(.LC" << lc_n << ")(" << context.GetRegString(srcReg) << ")" << std::endl;
        }
        else
        {
            throw std::runtime_error("FloatConstant::EmitRISC called with non-float type");
        }
        context.FreeRegister(srcReg);
    }

    std::any FloatConstant::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::FLOAT)
        {
            return static_cast<float>(value_);
        }
        else if (type == TypeSpecifier::DOUBLE)
        {
            return value_;
        }
        else
        {
            throw std::runtime_error("FloatConstant::GetValue called with non-float type");
        }
    }

    void FloatConstant::Print(std::ostream &stream) const
    {
        stream << raw_str_;
    }

    TypeSpecifier FloatConstant::GetType(Context &context) const
    {
        (void)context;
        if (raw_str_[raw_str_.size() - 1] == 'f')
        {
            return TypeSpecifier::FLOAT;
        }
        else
        {
            return TypeSpecifier::DOUBLE;
        }
    }

    int EscapedCharMap(std::string s)
    {
        if (s.substr(1, 3) == "\\x")
        {
            return std::stoi(s.substr(2), nullptr, 16);
        }
        else
        {
            char c = s[2];
            switch (c)
            {
            case 'n':
                return 10;
            case 't':
                return 9;
            case 'v':
                return 11;
            case 'b':
                return 8;
            case 'r':
                return 13;
            case 'f':
                return 12;
            case 'a':
                return 7;
            case '\\':
                return 92;
            case '\'':
                return 39;
            case '\"':
                return 34;
            case '?':
                return 63;
            default:
                throw std::runtime_error("Invalid escape character");
            }
        }
    }

    // initialize char int value;
    void CharLiteral::Char2Int()
    {
        // assume position 0 is the literal ' char
        if (raw_str_[1] == '\\')
        {
            value_ = EscapedCharMap(raw_str_);
        }
        else
        {
            value_ = static_cast<int>(raw_str_[1]);
        }
    }

    void CharLiteral::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        if (type != TypeSpecifier::CHAR && type != TypeSpecifier::INT)
        {
            throw std::runtime_error("CharLiteral::EmitRISC called with non-char or int type");
        }
        stream << "li " << context.GetRegString(destReg) << ", " << value_ << std::endl;
    }

    void CharLiteral::Print(std::ostream &stream) const
    {
        stream << raw_str_;
    }

    std::any CharLiteral::GetValue(TypeSpecifier type) const
    {
        if (type == TypeSpecifier::CHAR)
        {
            return value_;
        }
        else
        {
            throw std::runtime_error("CharLiteral::GetValue called with non-char type");
        }
    }

    TypeSpecifier CharLiteral::GetType(Context &context) const
    {
        (void)context;
        return TypeSpecifier::CHAR;
    }

    void StringLiteral::EmitRISC(std::ostream &stream, Context &context, int destReg, TypeSpecifier type) const
    {
        (void)type;
        int lc_n = context.AddStringLiteralConstant(raw_str_);
        stream << "lui " << context.GetRegString(destReg) << ", %hi(.LC" << lc_n << ")" << std::endl;
        stream << "addi " << context.GetRegString(destReg) << ", " << context.GetRegString(destReg) << ", %lo(.LC" << lc_n << ")" << std::endl;
    }

    void StringLiteral::Print(std::ostream &stream) const
    {
        stream << raw_str_;
    }

    std::string StringLiteral::GetID() const
    {
        return raw_str_;
    }

    TypeSpecifier StringLiteral::GetType(Context &context) const
    {
        (void)context;
        return TypeSpecifier::STRING;
    }

    int StringLiteral::GetArraySize(Context &context) const
    {
        (void)context;
        // -1 to remove the quotes but include the null terminator
        return raw_str_.size() - 1;
    }

} // namespace ast
