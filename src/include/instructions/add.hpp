#pragma once
#include <string>
#include <variant>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"


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
    namespace instructions
    {
        struct Add : public Instruction
        {
            type::Type m_result_type;
            virtual vm::Error execute(vm::VM &vm) override;

            Add(type::Type result_type) : m_result_type(result_type) {}
        };

#ifdef SCC_ADD_IMPLEMENTATION
        vm::Error Add::execute(vm::VM &vm)
        {
            uint64_t a{0}, b{0};

            auto error = vm.vm_stack_pop(a);
            if (error != vm::Error::None)
                return error;

            error = vm.vm_stack_pop(b);
            if (error != vm::Error::None)
                return error;

            // TODOOO: we probably want more add instuctions, like add_i32, add_f32, etc?
            std::visit(overloaded{[&](const type::I32 &)
                                  {
                                      auto result = static_cast<int32_t>(a) + static_cast<int32_t>(b);
                                      error = vm.vm_stack_push(static_cast<uint64_t>(result));
                                      if (error != vm::Error::None)
                                          return error;

                                      return vm::Error::None;
                                  },
                                  [&](const type::F32 &)
                                  {
                                      auto result = static_cast<float>(a) + static_cast<float>(b);
                                      error = vm.vm_stack_push(static_cast<uint64_t>(result));
                                      if (error != vm::Error::None)
                                          return error;

                                      return vm::Error::None;
                                  },
                                  [&](const type::PTR &)
                                  {
                                      assert(false && "Unimplemented type");
                                      return vm::Error::None;
                                  }

                       },
                       m_result_type.kind);

            return vm::Error::None;
        }
#endif
    }
}