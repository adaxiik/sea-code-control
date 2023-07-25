#pragma once
#include <variant>
#include <memory>
#include <iostream>
#include <optional>
namespace scc
{
    namespace type
    {
        struct GetSizeBytes
        {
            template <class T>
            size_t operator()(const T &t) const
            {
                return t.size_bytes();
            }
        };

        struct OstreamPrint
        {
            std::ostream &os;
            template <class T>
            void operator()(const T &t) const
            {
                os << t;
            }
        };

        struct Type;

        struct ptr_type
        {
            // Type *pointing_to;
            std::shared_ptr<Type> pointing_to;
            size_t size;
            bool is_static_array;
            
            ptr_type(std::shared_ptr<Type> pointing_to, size_t size = 1, bool is_static_array = false)
                : pointing_to(pointing_to), size(size), is_static_array(is_static_array)
            {
            }

            size_t size_bytes() const;
            friend std::ostream &operator<<(std::ostream &os, const ptr_type &);
        };

        // types: i64, i32, i16, i8, u64, u32, u16, u8, _Bool, ptr, static array, struct, union, enum, function ptr
        #define DECLARE_TYPE(NAME, SIZE_BYTES, STREAM_PRINT) \
        struct NAME##_type \
        { \
            size_t size_bytes() const { return SIZE_BYTES; } \
            friend std::ostream &operator<<(std::ostream &os, const NAME##_type &) \
            { \
                os << STREAM_PRINT; \
                return os; \
            } \
        };
    
        DECLARE_TYPE(i8, 1UL, "signed char")
        DECLARE_TYPE(u8, 1UL, "char")
        DECLARE_TYPE(i16, 2UL, "short")
        DECLARE_TYPE(u16, 2UL, "unsigned short")
        DECLARE_TYPE(i32, 4UL, "int")
        DECLARE_TYPE(u32, 4UL, "unsigned int")
        DECLARE_TYPE(i64, 8UL, "long")
        DECLARE_TYPE(u64, 8UL, "unsigned long")
        DECLARE_TYPE(f32, 4UL, "float")
        DECLARE_TYPE(f64, 8UL, "double")
        DECLARE_TYPE(boolean, 1UL, "bool")
        


        struct Type
        {
            using Kind = std::variant<
                i8_type,
                u8_type,
                i16_type,
                u16_type,
                i32_type,
                u32_type,
                i64_type,
                u64_type,
                f32_type,
                f64_type,
                boolean_type,
                ptr_type
                >;

            Kind kind;
            bool is_const;

            Type(Kind kind, bool is_const = false)
                : kind(kind), is_const(is_const)
            {
            }

            Type() = default;


            // Type(const Type &other)
            //     : kind(other.kind), is_const(other.is_const)
            // {
            // }

            // Type &operator=(const Type &other)
            // {
            //     kind = other.kind;
            //     is_const = other.is_const;
            //     return *this;
            // }

            size_t size_bytes() const
            {
                return std::visit(GetSizeBytes{}, kind);
            }

            friend std::ostream &operator<<(std::ostream &os, const Type &type)
            {
                if (type.is_const)
                    os << "const ";

                std::visit(OstreamPrint{os}, type.kind);

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
                return kind.index() == other.kind.index();
            }

            bool operator!=(const Type &other) const
            {
                return !(*this == other);
            }

            // bool is_int()    const { return std::holds_alternative<i32_type>(kind); }
            // bool is_float()  const { return std::holds_alternative<f32_type>(kind); }
            // bool is_double() const { return std::holds_alternative<f64_type>(kind); }
        };

    }
}