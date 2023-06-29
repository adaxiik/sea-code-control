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
            std::unique_ptr<Type> pointing_to;
            size_t size;
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
                std::visit(overloaded{
                    [&os](const I32& ) { os << "i32"; },
                    [&os](const F32& ) { os << "f32"; },
                    [&os](const PTR& ptr) { os << "ptr<" << *ptr.pointing_to << ">"; }
                }, type.kind);
                return os;
            }
        };

    }
}