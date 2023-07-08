#pragma once
#include <string>
#include <variant>
#include "type.hpp"
#include "vm.hpp"
#include "./instruction.hpp"

namespace scc
{
    namespace instructions
    {
        struct Add : public Instruction
        {
            type::Type m_result_type;
            virtual vm::Error execute(vm::VM &vm) override;

            Add(type::Type result_type) : m_result_type(result_type) {}

            virtual void accept(Visitor &v) const override { v.visit(*this); }
        };

#ifdef SCC_ADD_IMPLEMENTATION
        vm::Error Add::execute(vm::VM &vm)
        {
            vm::Value a{0}, b{0};

            auto error = vm.vm_stack_pop(a);
            if (error != vm::Error::None)
                return error;

            error = vm.vm_stack_pop(b);
            if (error != vm::Error::None)
                return error;

            static_assert(std::variant_size_v<scc::type::Type::Kind> == 4, "Add instruction not implemented for all types");

            // TODOOO: we probably want more add instructions, like add_i32, add_f32, etc?
            std::visit(overloaded{[&](const type::int_type &)
                                  {
                                      int result = a.as<int>() + b.as<int>();
                                      error = vm.vm_stack_push(result);
                                      if (error != vm::Error::None)
                                          return error;

                                      return vm::Error::None;
                                  },
                                  [&](const type::float_type &)
                                  {
                                      float result = a.as<float>() + b.as<float>();
                                      error = vm.vm_stack_push(result);
                                      if (error != vm::Error::None)
                                          return error;

                                      return vm::Error::None;
                                  },
                                  [&](const type::ptr_type &)
                                  {
                                      assert(false && "Add instruction not implemented for pointers");
                                      return vm::Error::None;
                                  },
                                  [&](const type::double_type &)
                                  {
                                      double result = a.as<double>() + b.as<double>();
                                      error = vm.vm_stack_push(result);
                                      if (error != vm::Error::None)
                                          return error;

                                      return vm::Error::None;
                                  }

                       },
                       m_result_type.kind);

            return vm::Error::None;
        }
#endif
    }
}