#pragma once
#include <stack>
#include "memory.hpp"
#include "interpreter_scope.hpp"
namespace scc
{
    class CallStack
    {
        using InstructionPointer = size_t;

        struct CallFrame
        {
            InstructionPointer return_address;
            InterpreterScopeStack scope_stack;
            CallFrame(InstructionPointer return_address, Memory::address_t start_address)
                : return_address(return_address), scope_stack(InterpreterScopeStack(start_address)) {}
        };

        std::stack<CallFrame> m_call_stack;

    public:
        CallStack(Memory::address_t base_adress)
        {
            m_call_stack.push(CallFrame(0, base_adress));
        }

        auto& scope_stack()
        {
            return m_call_stack.top().scope_stack;
        }

        void push(InstructionPointer return_address)
        {
            m_call_stack.push(CallFrame(return_address, scope_stack().current_address()));
        }

        InstructionPointer pop()
        {
            auto return_address = m_call_stack.top().return_address;
            m_call_stack.pop();
            return return_address;
        }

        void push_scope()
        {
            m_call_stack.top().scope_stack.push();
        }

        void pop_scope()
        {
            m_call_stack.top().scope_stack.pop();
        }

        std::stack<CallFrame> copy_call_stack() const
        {
            return m_call_stack;
        }

    };
}