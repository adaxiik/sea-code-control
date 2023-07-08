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
        struct Cast : public Instruction
        {
            type::Type m_from_type;
            type::Type m_to_type;
            virtual vm::Error execute(vm::VM &vm) override;

            Cast(type::Type from_type, type::Type to_type) : m_from_type(from_type), m_to_type(to_type) {}

            virtual void accept(InstructionVisitor &v) const override { v.visit(*this); }
        };

#ifdef SCC_CAST_IMPLEMENTATION
        // TODOOOOOOO: templated function instead of macro
        #define SCC_CAST_FT_IMPLEMENTATION(from, to)         \
        do{                                                  \
            to result = static_cast<to>(buffer.as<from>());  \
            auto error_ = vm.vm_stack_push(result);          \
            if (error_ != vm::Error::None)                   \
                return error_;                               \
            return vm::Error::None;                          \
        } while(0) 

        #define SCC_CAST_UNIMPLEMENTED_ERROR \
        do{                                  \
            assert(false && "Cast instruction not implemented for pointers"); \
            return vm::Error::None;          \
        } while(0)



        vm::Error Cast::execute(vm::VM &vm)
        {
            vm::Value buffer{0};

            auto error = vm.vm_stack_pop(buffer);
            if (error != vm::Error::None)
                return error;
            static_assert(std::variant_size_v<scc::type::Type::Kind> == 4, "Cast instruction not implemented for all types");
            
            // TODOOOO: something better than cartesian product?
            std::visit(overloaded{
                [&](const type::int_type &)
                {
                    std::visit(overloaded{
                        [&](const type::int_type &)    { SCC_CAST_FT_IMPLEMENTATION(int, int);    },
                        [&](const type::float_type &)  { SCC_CAST_FT_IMPLEMENTATION(int, float);  },
                        [&](const type::double_type &) { SCC_CAST_FT_IMPLEMENTATION(int, double); },
                        [&](const type::ptr_type &)    { SCC_CAST_UNIMPLEMENTED_ERROR; },
                    },m_to_type.kind);
                },
                [&](const type::float_type &)
                {
                    std::visit(overloaded{
                        [&](const type::int_type &)    { SCC_CAST_FT_IMPLEMENTATION(float, int);    },
                        [&](const type::float_type &)  { SCC_CAST_FT_IMPLEMENTATION(float, float);  },
                        [&](const type::double_type &) { SCC_CAST_FT_IMPLEMENTATION(float, double); },
                        [&](const type::ptr_type &)    { SCC_CAST_UNIMPLEMENTED_ERROR; },
                    },m_to_type.kind);
                },
                [&](const type::double_type &)
                {
                    std::visit(overloaded{
                        [&](const type::int_type &)    { SCC_CAST_FT_IMPLEMENTATION(double, int);    },
                        [&](const type::float_type &)  { SCC_CAST_FT_IMPLEMENTATION(double, float);  },
                        [&](const type::double_type &) { SCC_CAST_FT_IMPLEMENTATION(double, double); },
                        [&](const type::ptr_type &)    { SCC_CAST_UNIMPLEMENTED_ERROR; },
                    },m_to_type.kind);
                },
                [&](const type::ptr_type &)
                {
                    assert(false && "Cast instruction not implemented for pointers");
                },
            },m_from_type.kind);


            return vm::Error::None;
        }
#endif
    }
}