#include <iostream>
#include <variant>
#include "lowering/inbuild_functions.hpp"
#include "interpreter_io.hpp"

namespace scc::lowering::inbuild
{
    InterpreterResult puti(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::Kind::I32))
            return InterpreterError::ReachedUnreachableError;
        
        // you never know
        if (not std::holds_alternative<Type::Primitive::I32>(value.get_value().value))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::I32 val = std::get<Type::Primitive::I32>(value.get_value().value);
        InterpreterIO::write_stdout(std::to_string(val) + "\n");

        return InterpreterError::None;
    }

    InterpreterResult putc(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::Kind::Char))
            return InterpreterError::ReachedUnreachableError;
        
        // you never know
        if (not std::holds_alternative<Type::Primitive::Char>(value.get_value().value))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::Char val = std::get<Type::Primitive::Char>(value.get_value().value);
        InterpreterIO::write_stdout(std::string(1, val) + "\n");

        return InterpreterError::None;  
    }
}

