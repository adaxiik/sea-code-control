#pragma once
#include <variant>
#include <memory>
#include <iostream>
#include <optional>
#include "operation_result.hpp"
namespace scc
{
    // namespace type
    // {
    // struct GetSizeBytes
    // {
    //     template <class T>
    //     size_t operator()(const T &t) const
    //     {
    //         return t.size_bytes();
    //     }
    // };

    // struct OstreamPrint
    // {
    //     std::ostream &os;
    //     template <class T>
    //     void operator()(const T &t) const
    //     {
    //         os << t;
    //     }
    // };

    struct Type;

    //         struct ptr_type
    //         {
    //             // Type *pointing_to;
    //             std::shared_ptr<Type> pointing_to;
    //             size_t size;
    //             bool is_static_array;

    //             ptr_type(std::shared_ptr<Type> pointing_to, size_t size = 1, bool is_static_array = false)
    //                 : pointing_to(pointing_to), size(size), is_static_array(is_static_array)
    //             {
    //             }

    //             size_t size_bytes() const;
    //             friend std::ostream &operator<<(std::ostream &os, const ptr_type &);
    //         };

    // // types: i64, i32, i16, i8, u64, u32, u16, u8, _Bool, ptr, static array, struct, union, enum, function ptr
    /*#define DECLARE_TYPE(NAME, SIZE_BYTES, CTYPE_ALTERNATIVE)                      \
    //     struct NAME##_type                                                         \
    //     {                                                                          \
    //         static_assert(sizeof(CTYPE_ALTERNATIVE) == SIZE_BYTES, "Invalid size");\
    //         using CTypeAlternative = CTYPE_ALTERNATIVE;                            \
    //         size_t size_bytes() const { return SIZE_BYTES; }                       \
    //         friend std::ostream &operator<<(std::ostream &os, const NAME##_type &) \
    //         {                                                                      \
    //             os << #CTYPE_ALTERNATIVE;                                          \
    //             return os;                                                         \
    //         }                                                                      \
    //     };
    //         DECLARE_TYPE(char, 1UL, char)
    //         DECLARE_TYPE(i8, 1UL, signed char)
    //         DECLARE_TYPE(u8, 1UL, unsigned char)
    //         DECLARE_TYPE(i16, 2UL, signed short)
    //         DECLARE_TYPE(u16, 2UL, unsigned short)
    //         DECLARE_TYPE(i32, 4UL, signed int)
    //         DECLARE_TYPE(u32, 4UL, unsigned int)
    //         DECLARE_TYPE(i64, 8UL, signed long)
    //         DECLARE_TYPE(u64, 8UL, unsigned long)
    //         DECLARE_TYPE(f32, 4UL, float)
    //         DECLARE_TYPE(f64, 8UL, double)
    //         DECLARE_TYPE(boolean, 1UL, bool)
    */
    struct Type
    {
        // using Kind = std::variant<
        //     char_type,
        //     i8_type,
        //     u8_type,
        //     i16_type,
        //     u16_type,
        //     i32_type,
        //     u32_type,
        //     i64_type,
        //     u64_type,
        //     f32_type,
        //     f64_type,
        //     boolean_type,
        //     ptr_type>;

        enum class Kind
        {
            Char = 0,
            I8,
            U8,
            I16,
            U16,
            I32,
            U32,
            I64,
            U64,
            F32,
            F64,
            Bool,
            COUNT
        };

        Kind kind;
        // bool is_const;

        // Type(Kind kind, bool is_const = false)
        //     : kind(kind), is_const(is_const)
        // {
        // }
        constexpr Type(Kind kind) : kind(kind) {}

        // constexpr Type() = default;

        constexpr size_t size_bytes() const
        {
            // return std::visit(GetSizeBytes{}, kind);
            switch (kind)
            {
            case Kind::Char:
                return sizeof(char);
            case Kind::I8:
                return sizeof(signed char);
            case Kind::U8:
                return sizeof(unsigned char);
            case Kind::I16:
                return sizeof(signed short);
            case Kind::U16:
                return sizeof(unsigned short);
            case Kind::I32:
                return sizeof(signed int);
            case Kind::U32:
                return sizeof(unsigned int);
            case Kind::I64:
                return sizeof(signed long);
            case Kind::U64:
                return sizeof(unsigned long);
            case Kind::F32:
                return sizeof(float);
            case Kind::F64:
                return sizeof(double);
            case Kind::Bool:
                return sizeof(bool);
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
                return 0;
            }
        }

        friend std::ostream &operator<<(std::ostream &os, const Type &type)
        {
            // if (type.is_const)
            //     os << "const ";

            // std::visit(OstreamPrint{os}, type.kind);
            static_assert(static_cast<size_t>(Kind::COUNT) == 12, "Update this code");
            switch (type.kind)
            {
            case Kind::Char:
                os << "char";
                break;
            case Kind::I8:
                os << "signed char";
                break;
            case Kind::U8:
                os << "unsigned char";
                break;
            case Kind::I16:
                os << "signed short";
                break;
            case Kind::U16:
                os << "unsigned short";
                break;
            case Kind::I32:
                os << "signed int";
                break;
            case Kind::U32:
                os << "unsigned int";
                break;
            case Kind::I64:
                os << "signed long";
                break;
            case Kind::U64:
                os << "unsigned long";
                break;
            case Kind::F32:
                os << "float";
                break;
            case Kind::F64:
                os << "double";
                break;
            case Kind::Bool:
                os << "bool";
                break;
            default:
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            };

            return os;
        }

        // static std::optional<Type> from_string(const std::string &str)
        // {
        //     if (str == "int")
        //         return Type(i32_type{});
        //     else if (str == "float")
        //         return Type(f32_type{});
        //     else if (str == "double")
        //         return Type(f64_type{});
        //     else
        //         return std::nullopt;
        // }

        bool operator==(const Type &other) const
        {
            // return kind.index() == other.kind.index();
            return kind == other.kind;
        }

        bool operator!=(const Type &other) const
        {
            return !(*this == other);
        }

        // bool is_int()    const { return std::holds_alternative<i32_type>(kind); }
        // bool is_float()  const { return std::holds_alternative<f32_type>(kind); }
        // bool is_double() const { return std::holds_alternative<f64_type>(kind); }
        // template <typename T>
        // static constexpr bool is_valid_type()
        // {
        //     return std::is_same_v<T, char_type>    ||
        //            std::is_same_v<T, i8_type>      ||
        //            std::is_same_v<T, u8_type>      ||
        //            std::is_same_v<T, i16_type>     ||
        //            std::is_same_v<T, u16_type>     ||
        //            std::is_same_v<T, i32_type>     ||
        //            std::is_same_v<T, u32_type>     ||
        //            std::is_same_v<T, i64_type>     ||
        //            std::is_same_v<T, u64_type>     ||
        //            std::is_same_v<T, f32_type>     ||
        //            std::is_same_v<T, f64_type>     ||
        //            std::is_same_v<T, boolean_type> ||
        //            std::is_same_v<T, ptr_type>;
        // }

        // template <typename T>
        // static Type create_type(bool is_const = false)
        // {
        //     static_assert(Type::is_valid_type<T>(), "Invalid type");
        //     return Type(T(), is_const);
        // }

        template <typename T>
        static constexpr Type deduce_type()
        {
            // if constexpr (std::is_same_v<T, char>)
            //     return create_type<char_type>(is_const);
            // else if constexpr (std::is_same_v<T, signed char>)
            //     return create_type<i8_type>(is_const);
            // else if constexpr (std::is_same_v<T, unsigned char>)
            //     return create_type<u8_type>(is_const);
            // else if constexpr (std::is_same_v<T, short>)
            //     return create_type<i16_type>(is_const);
            // else if constexpr (std::is_same_v<T, unsigned short>)
            //     return create_type<u16_type>(is_const);
            // else if constexpr (std::is_same_v<T, int>)
            //     return create_type<i32_type>(is_const);
            // else if constexpr (std::is_same_v<T, unsigned int>)
            //     return create_type<u32_type>(is_const);
            // else if constexpr (std::is_same_v<T, long>)
            //     return create_type<i64_type>(is_const);
            // else if constexpr (std::is_same_v<T, unsigned long>)
            //     return create_type<u64_type>(is_const);
            // else if constexpr (std::is_same_v<T, float>)
            //     return create_type<f32_type>(is_const);
            // else if constexpr (std::is_same_v<T, double>)
            //     return create_type<f64_type>(is_const);
            // else if constexpr (std::is_same_v<T, bool>)
            //     return create_type<boolean_type>(is_const);
            // else if constexpr (std::is_pointer_v<T>)
            //     return Type(ptr_type(Type::deduce_type<std::remove_pointer_t<T>>(), 1, false), is_const);
            // else
            //     static_assert(!sizeof(T), "Invalid type");

            if constexpr (std::is_same_v<T, char>)
                return Type(Kind::Char);
            else if constexpr (std::is_same_v<T, signed char>)
                return Type(Kind::I8);
            else if constexpr (std::is_same_v<T, unsigned char>)
                return Type(Kind::U8);
            else if constexpr (std::is_same_v<T, short>)
                return Type(Kind::I16);
            else if constexpr (std::is_same_v<T, unsigned short>)
                return Type(Kind::U16);
            else if constexpr (std::is_same_v<T, int>)
                return Type(Kind::I32);
            else if constexpr (std::is_same_v<T, unsigned int>)
                return Type(Kind::U32);
            else if constexpr (std::is_same_v<T, long>)
                return Type(Kind::I64);
            else if constexpr (std::is_same_v<T, long long>)
                return Type(Kind::I64);
            else if constexpr (std::is_same_v<T, unsigned long>)
                return Type(Kind::U64);
            else if constexpr (std::is_same_v<T, unsigned long long>)
                return Type(Kind::U64);
            else if constexpr (std::is_same_v<T, float>)
                return Type(Kind::F32);
            else if constexpr (std::is_same_v<T, double>)
                return Type(Kind::F64);
            else if constexpr (std::is_same_v<T, bool>)
                return Type(Kind::Bool);
            else if constexpr (std::is_same_v<T, OperationResult::InvalidOperation>)
            {
                std::cerr << "UNREACHABLE at " << __FILE__ << ":" << __LINE__ << std::endl;
                std::exit(1);
            }
            else
                static_assert(!sizeof(T), "Invalid type");

            return Type(Kind::COUNT);
        }


        
       




    };

    // }
}