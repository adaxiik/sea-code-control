#pragma once
#include "vm.hpp"
#include "visitor.hpp"

// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


namespace scc
{
    struct Instruction
    {
        virtual vm::Error execute(vm::VM& vm) = 0;
        virtual ~Instruction() = default;

        virtual void accept(Visitor& ) const = 0;
    };

}
