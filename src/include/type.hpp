#pragma once
#include <variant>
#include <memory>
#include <iostream>
namespace scc
{
    namespace type
    {
        #define scc_type_Kind std::variant<I32, F32, PTR>
        struct Type;

        struct I32{};
        struct F32{};

        struct PTR
        {
            Type* pointing_to;
            size_t size;
            bool is_static_array;
        };

        template <class... Ts>
        struct overloaded : Ts...
        {
            using Ts::operator()...;
        };
        template <class... Ts>
        overloaded(Ts...) -> overloaded<Ts...>;

        
        struct Type
        {
            scc_type_Kind kind;
            bool is_const;

            friend std::ostream& operator<<(std::ostream& os, const Type& type)
            {
                if (type.is_const)
                    os << "const ";

                std::visit(overloaded{
                    [&os](const I32& ) { os << "i32"; },
                    [&os](const F32& ) { os << "f32"; },
                    [&os](const PTR& ptr) { os << *ptr.pointing_to << "*"; }
                }, type.kind);
                return os;
            }

            size_t size_bytes() const
            {
                return std::visit(overloaded{
                    [](const I32& ) { return 4UL; },
                    [](const F32& ) { return 4UL; },
                    [](const PTR& ptr) 
                    { 
                        if (ptr.is_static_array)
                            return static_cast<size_t>(ptr.size * ptr.pointing_to->size_bytes());
                        else
                            return 8UL;
                    }
                }, kind);
            }

            Type(scc_type_Kind kind, bool is_const = false)
                : kind(kind)
                , is_const(is_const)
            {}
            Type() = default;

        };

    }
}