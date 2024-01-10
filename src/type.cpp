#include "type.hpp"

namespace scc
{
    Type Type::from_value(const Value &value)
    {
        return value.type;
    }

    bool Type::operator==(const Type &other) const
    {
        if (base_type != other.base_type)
            return false;

        if (modifiers.size() != other.modifiers.size())
            return false;

        for (size_t i = 0; i < modifiers.size(); i++)
        {
            bool modifiers_equal = std::visit(
                overloaded{
                    [&](Pointer)
                    { return std::holds_alternative<Pointer>(other.modifiers[i]); },
                    [&](Array a)
                    { return std::holds_alternative<Array>(other.modifiers[i]) and a.size == std::get<Array>(other.modifiers[i]).size; }},
                modifiers[i]);

            if (not modifiers_equal)
                return false;
        }

        return true;
    }

    bool Type::operator!=(const Type &other) const
    {
        return not(*this == other);
    }

    std::optional<Type> Type::from_string(const std::string &str)
    {
        if (str == "char")
            return Type(PrimitiveType::Char);
        else if (str == "signed char")
            return Type(PrimitiveType::I8);
        else if (str == "unsigned char")
            return Type(PrimitiveType::U8);
        else if (str == "signed short")
            return Type(PrimitiveType::I16);
        else if (str == "unsigned short")
            return Type(PrimitiveType::U16);
        else if (str == "signed int")
            return Type(PrimitiveType::I32);
        else if (str == "unsigned int")
            return Type(PrimitiveType::U32);
        else if (str == "signed long")
            return Type(PrimitiveType::I64);
        else if (str == "unsigned long")
            return Type(PrimitiveType::U64);
        else if (str == "float")
            return Type(PrimitiveType::F32);
        else if (str == "double")
            return Type(PrimitiveType::F64);
        // TODOOOO: change to _Bool and check it in bindings.. its probably not parsed as primitive type
        else if (str == "bool")
            return Type(PrimitiveType::Bool);
        else if (str == "int")
            return Type(PrimitiveType::I32);
        else if (str == "long")
            return Type(PrimitiveType::I64);
        else if (str == "short")
            return Type(PrimitiveType::I16);
        else if (str == "unsigned")
            return Type(PrimitiveType::U32);
        else if (str == "signed")
            return Type(PrimitiveType::I32);
        else if (str == "void")
            return Type(PrimitiveType::Void);
        else
            return std::nullopt;
    }

    // friend std::ostream &operator<<(std::ostream &os, const Type &type);
    std::ostream &operator<<(std::ostream &os, const Type &type)
    {
        std::visit(overloaded{
            [&](const Type::PrimitiveType &type)
            {
                static_assert(static_cast<int>(Type::PrimitiveType::COUNT) == 13, "Update this code");


                switch (type)
                {
                    case Type::PrimitiveType::Char: os << "char";           break;
                    case Type::PrimitiveType::U8:   os << "unsigned char";  break;
                    case Type::PrimitiveType::U16:  os << "unsigned short"; break;
                    case Type::PrimitiveType::U32:  os << "unsigned int";   break;
                    case Type::PrimitiveType::U64:  os << "unsigned long";  break;
                    case Type::PrimitiveType::I8:   os << "signed char";    break;
                    case Type::PrimitiveType::I16:  os << "short";          break;
                    case Type::PrimitiveType::I32:  os << "int";            break;
                    case Type::PrimitiveType::I64:  os << "long";           break;
                    case Type::PrimitiveType::F32:  os << "float";          break;
                    case Type::PrimitiveType::F64:  os << "double";         break;
                    case Type::PrimitiveType::Bool: os << "bool";           break;
                    case Type::PrimitiveType::Void: os << "void";           break;
                    default: os << "Unreachable " << __FILE__ << ":" << __LINE__; std::exit(1); break;
                }
            },
            [&](const Type::StructType &type)
            {
                os << "struct " << type.name;
            }},
    type.base_type);

            // TODOOO: Handle arrays and pointers
            for (size_t i = 0; i < type.modifiers.size(); i++)
                os << "*";           

        return os;
    }

}