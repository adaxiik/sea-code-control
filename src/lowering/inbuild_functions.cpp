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

    InterpreterResult assert(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::Bool))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::Bool>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::Bool val = std::get<Type::Primitive::Bool>(value.get_value().value.primitive_value().value());
        
        if (not val)
            return InterpreterError::AssertionFailedError;
    
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

    InterpreterResult malloc(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::U64))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::U64>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::U64 size = std::get<Type::Primitive::U64>(value.get_value().value.primitive_value().value());
        Type::Primitive::PTR addr = state.memory.allocate(size);

        return InterpreterResult::ok(InterpreterResultValue(addr, Type(Type::PrimitiveType::Void, std::vector<Type::Modifier>{Type::Pointer{}})));
    }

    InterpreterResult free(InterpreterState &state)
    {
        InterpreterResult value = state.result_stack.top();
        state.result_stack.pop();

        if (not value.is_ok_and_has_value())
            return value;

        if (value.get_value().type != Type(Type::PrimitiveType::Void, std::vector<Type::Modifier>{Type::Pointer{}}))
            return InterpreterError::ReachedUnreachableError;
        
        if (not std::holds_alternative<Type::Primitive::PTR>(value.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        Type::Primitive::PTR addr = std::get<Type::Primitive::PTR>(value.get_value().value.primitive_value().value());
        
        if (not state.memory.free(addr))
            return InterpreterError::MemoryError;

        return InterpreterError::None;
    }

    InterpreterResult write(InterpreterState &state)
    {
        if (state.result_stack.size() < 3)
            return InterpreterError::RuntimeError;
        
        InterpreterResult fd = state.result_stack.top();
        state.result_stack.pop();

        InterpreterResult addr = state.result_stack.top();
        state.result_stack.pop();

        InterpreterResult count = state.result_stack.top();
        state.result_stack.pop();

        if (not count.is_ok_and_has_value())
            return count;

        if (not addr.is_ok_and_has_value())
            return addr;

        if (not fd.is_ok_and_has_value())
            return fd;

        if (count.get_value().type != Type(Type::PrimitiveType::U64))
            return InterpreterError::ReachedUnreachableError;

        if (not std::holds_alternative<Type::Primitive::U64>(count.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;
        
        if (addr.get_value().type != Type(Type::PrimitiveType::Void, std::vector<Type::Modifier>{Type::Pointer{}}))
            return InterpreterError::ReachedUnreachableError;

        if (not std::holds_alternative<Type::Primitive::PTR>(addr.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;

        if (fd.get_value().type != Type(Type::PrimitiveType::I32))
            return InterpreterError::ReachedUnreachableError;

        if (not std::holds_alternative<Type::Primitive::I32>(fd.get_value().value.primitive_value().value()))
            return InterpreterError::ReachedUnreachableError;

        Type::Primitive::U64 count_val = std::get<Type::Primitive::U64>(count.get_value().value.primitive_value().value());
        Type::Primitive::PTR addr_val = std::get<Type::Primitive::PTR>(addr.get_value().value.primitive_value().value());
        Type::Primitive::I32 fd_val = std::get<Type::Primitive::I32>(fd.get_value().value.primitive_value().value());

        std::unique_ptr<char[]> data = std::make_unique<char[]>(count_val);

        if (not state.memory.read_into(addr_val, data.get(), count_val))
            return InterpreterError::MemoryError;


        if (fd_val == 1)
            InterpreterIO::write_stdout(std::string(data.get(), count_val));
        else if (fd_val == 2)
            InterpreterIO::write_stderr(std::string(data.get(), count_val));
        else
            return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::I64>(-1)));
            
        return InterpreterResult::ok(InterpreterResultValue(static_cast<Type::Primitive::I64>(count_val)));
    }

    InterpreterResult read(InterpreterState &)
    {
        return InterpreterError::None;
    }

}

