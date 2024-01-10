#include <iostream>
#include <variant>
#include <cmath>

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

        if (value.get_value().type != Type(Type::PrimitiveType::I32))
            return InterpreterError::ReachedUnreachableError;
        
        // you never know
        if (not std::holds_alternative<Type::Primitive::I32>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::I32 val = std::get<Type::Primitive::I32>(value.get_value().value.primitive_value().value());
        InterpreterIO::write_stdout(std::to_string(val) + "\n");

        return InterpreterError::None;
    }

    InterpreterResult putc(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::Char))
            return InterpreterError::ReachedUnreachableError;
        
        // you never know
        if (not std::holds_alternative<Type::Primitive::Char>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::Char val = std::get<Type::Primitive::Char>(value.get_value().value.primitive_value().value());
        InterpreterIO::write_stdout(std::string(1, val));

        return InterpreterError::None;  
    }

    InterpreterResult sinf(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::F32))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::F32>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::F32 val = std::get<Type::Primitive::F32>(value.get_value().value.primitive_value().value());

        return InterpreterResult::ok(InterpreterResultValue(std::sin(val)));
    }

    InterpreterResult cosf(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::F32))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::F32>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::F32 val = std::get<Type::Primitive::F32>(value.get_value().value.primitive_value().value());

        return InterpreterResult::ok(InterpreterResultValue(std::cos(val)));
    }

    InterpreterResult sin(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::F64))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::F64>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::F64 val = std::get<Type::Primitive::F64>(value.get_value().value.primitive_value().value());

        return InterpreterResult::ok(InterpreterResultValue(std::sin(val)));
    }

    InterpreterResult cos(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::F64))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::F64>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::F64 val = std::get<Type::Primitive::F64>(value.get_value().value.primitive_value().value());

        return InterpreterResult::ok(InterpreterResultValue(std::cos(val)));
    }
}

