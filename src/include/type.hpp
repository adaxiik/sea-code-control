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

        struct int_type
        {
            size_t size_bytes() const { return 4UL; }

            friend std::ostream &operator<<(std::ostream &os, const int_type &)
            {
                os << "int";
                return os;
            }
        };

        struct float_type
        {
            size_t size_bytes() const { return 4UL; }

            friend std::ostream &operator<<(std::ostream &os, const float_type &)
            {
                os << "float";
                return os;
            }
        };

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

        struct double_type
        {
            size_t size_bytes() const { return 8UL; }

            friend std::ostream &operator<<(std::ostream &os, const double_type &)
            {
                os << "double";
                return os;
            }
        };

        struct Type
        {
            using Kind = std::variant<int_type, float_type, double_type, ptr_type>;
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

            static std::optional<Type> from_string(const std::string &str)
            {
                if (str == "int")
                    return Type(int_type{});
                else if (str == "float")
                    return Type(float_type{});
                else if (str == "double")
                    return Type(double_type{});
                else
                    return std::nullopt;
            }

            bool operator==(const Type &other) const
            {
                return kind.index() == other.kind.index();
            }

            bool operator!=(const Type &other) const
            {
                return !(*this == other);
            }

            bool is_int()    const { return std::holds_alternative<int_type>(kind); }
            bool is_float()  const { return std::holds_alternative<float_type>(kind); }
            bool is_double() const { return std::holds_alternative<double_type>(kind); }
        };

    }
}