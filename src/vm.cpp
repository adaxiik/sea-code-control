#include "vm.hpp"

#include <iostream>
#include <cstring>

namespace scc
{
    namespace vm
    {

        VM::VM(std::vector<Instruction> instructions)
            : m_stack{std::make_unique<char[]>(VM_STACK_SIZE)}
            , m_instructions{std::move(instructions)}
            , m_instruction_pointer{0}
            ,m_stack_pointer{VM_STACK_SIZE - (2 * Padding::Size)} 
        {
            m_scope_stack.push();
        }

        Error VM::execute_current()
        {

            std::visit(overloaded{
                            [&](const NewVar &new_var)
                            {
                                m_scope_stack.create_variable(new_var.name, Variable{new_var.type, m_stack_pointer});
                                m_stack_pointer -= new_var.type.size_bytes();
                                if (m_stack_pointer < 0)
                                    return Error::StackOverflow;

                                return Error::None;
                            },
                            [&](const PushScope &)
                            {
                                m_scope_stack.push();
                                return Error::None;
                            },
                            [&](const PopScope &)
                            {
                                m_scope_stack.pop();
                                return Error::None;
                            },
                            [&](const GetPtrToVar &get_ptr_to_var)
                            {
                                auto var = m_scope_stack[get_ptr_to_var.name];
                                if (!var.has_value())
                                    return Error::VariableDoesNotExist;
                                
                                auto error = stack_push(static_cast<uint64_t>(var->pointer));
                                if (error != Error::None)
                                    return error;
                                return Error::None;
                            },
                            [&](const Load &load)
                            {
                                assert(load.size_bytes <= 8 && "Load size is too big"); // for now.. do we even need more?
                                uint64_t ptr{0};
                                auto error = stack_pop(ptr);
                                if (error != Error::None)
                                    return error;
                                
                                if (ptr > VM_STACK_SIZE || static_cast<int64_t>(ptr) < 0) // todo: ignoring heap memory for now, we'll do memory tracing later
                                    return Error::IllegalMemoryAccess;
                                
                                uint64_t value{0};
                                std::memcpy(&value, m_stack.get() + ptr, load.size_bytes);
                                
                                error = stack_push(value);
                                if (error != Error::None)
                                    return error;
                                
                                return Error::None;
                            },
                            [&](const Store &store) 
                            {
                                // TODOO: check if not const somehow .. might use free bits int the pointer? because we are not going to alloc 2^64... something like nan boxing..
                                uint64_t ptr{0};
                                auto error = stack_pop(ptr);
                                if (error != Error::None)
                                    return error;
                                
                                if (ptr > VM_STACK_SIZE || static_cast<int64_t>(ptr) < 0) // TODOO: ignoring heap memory for now, we'll do memory tracing later
                                    return Error::IllegalMemoryAccess;

                                
                                uint64_t value{0};
                                error = stack_pop(value);
                                if (error != Error::None)
                                    return error;

                                
                                std::memcpy(m_stack.get() + ptr, &value, store.size_bytes);

                                std::cout << "store: " << value << " at: " << ptr << '\n';

                                return Error::None;
                            },
                            [&](const PushNumber &push_number)
                            {
                                auto error = stack_push(push_number.value);
                                if (error != Error::None)
                                    return error;

                                return Error::None;
                            },
                            [&](const Add & add)
                            {
                                uint64_t a{0}, b{0};

                                auto error = stack_pop(a);
                                if (error != Error::None)
                                    return error;
                                
                                error = stack_pop(b);
                                if (error != Error::None)
                                    return error;

                                std::cout << "a: " << a << " b: " << b << '\n';
                                
                                // indentation is starting to be annoying
                                std::visit(overloaded{
                                    [&](const type::I32 &)
                                    {
                                        auto result = static_cast<int32_t>(a) + static_cast<int32_t>(b);
                                        error = stack_push(static_cast<uint64_t>(result));
                                        if (error != Error::None)
                                            return error;
                                        
                                        return Error::None;
                                    },
                                    [&](const type::F32 &)
                                    {
                                        auto result = static_cast<float>(a) + static_cast<float>(b);
                                        error = stack_push(static_cast<uint64_t>(result));
                                        if (error != Error::None)
                                            return error;
                                        
                                        return Error::None;
                                    },
                                    [&](const type::PTR &)
                                    {
                                        assert(false && "Unimplemented type");
                                        return Error::None;
                                    }

                                }, add.result_type.kind);

                                return Error::None;
                            }
                       },
                       m_instructions[m_instruction_pointer]);

            return Error::None;
        }

        Error VM::run()
        {
            for (; m_instruction_pointer < m_instructions.size(); m_instruction_pointer++)
            {
                auto error = execute_current();
                if (error != Error::None)
                    return error;
            }

            return Error::None;
        }

        Error VM::pad_stack(size_t want_to_write_bytes)
        {
            // TODOOOO: investigate further for bugs.. Ill check it later again, after Ill have sizeof() and structures implemented
            size_t can_fit_bytes_on_single_write = Padding::Size - (m_stack_pointer % Padding::Size);
            if (can_fit_bytes_on_single_write >= want_to_write_bytes)
                return Error::None;

            size_t needed_writes = (want_to_write_bytes / Padding::Size) + static_cast<size_t>((want_to_write_bytes % Padding::Size) != 0);

            if (needed_writes == 1) // align to 4 bytes .. for example char, int32
            {
                m_stack_pointer -= can_fit_bytes_on_single_write;
                if (m_stack_pointer < 0)
                    return Error::StackOverflow;
                return Error::None;
            }

            m_stack_pointer -= m_stack_pointer % (Padding::Size * 2); // align to 8 bytes .. for example char, int64
            if (m_stack_pointer < 0)
                return Error::StackOverflow;

            return Error::None;
        }

        Error VM::stack_push(uint64_t value)
        {
            static_assert(sizeof(value) == 8, "Only 64 bit values are supported");
            const int size_of_value = 8;
            pad_stack(size_of_value);
            m_stack_pointer -= size_of_value;

            if (m_stack_pointer < 0)
                return Error::StackOverflow;

            // TODOOO: ub?
            // reinterpret_cast<uint64_t *>(&(m_stack[m_stack_pointer]))[0] = value; // hehe?  
            
            std::memcpy(&(m_stack[m_stack_pointer]), &value, size_of_value);

            return Error::None;
        }
        Error VM::stack_pop(uint64_t &value)
        {
            static_assert(sizeof(value) == 8, "Only 64 bit values are supported");
           
            value = 0;
            std::memcpy(&value, &(m_stack[m_stack_pointer]), sizeof(value));

            m_stack_pointer += 8;
            if (m_stack_pointer >= static_cast<int64_t>(VM_STACK_SIZE))
                return Error::StackUnderflow;

            return Error::None;
        }

    }
}
